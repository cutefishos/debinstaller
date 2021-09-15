/*
 * Copyright (C) 2021 Cutefish Technology Co., Ltd.
 *
 * Author:     Reion Wong <reion@cutefishos.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "debinstaller.h"
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QApt/Backend>

#include <QThread>
#include <QDebug>

static QString formatByteSize(double size, int precision)
{
    int unit = 0;
    double multiplier = 1024.0;

    while (qAbs(size) >= multiplier && unit < int(8)) {
        size /= multiplier;
        ++unit;
    }

    if (unit == 0) {
        precision = 0;
    }

    QString numString = QString::number(size, 'f', precision);

    switch (unit) {
    case 0:
        return QString("%1 B").arg(numString);
    case 1:
        return QString("%1 KB").arg(numString);
    case 2:
        return QString("%1 MB").arg(numString);
    case 3:
        return QString("%1 GB").arg(numString);
    case 4:
        return QString("%1 TB").arg(numString);
    case 5:
        return QString("%1 PB").arg(numString);
    case 6:
        return QString("%1 EB").arg(numString);
    case 7:
        return QString("%1 ZB").arg(numString);
    case 8:
        return QString("%1 YB").arg(numString);
    default:
        return QString();
    }

    return QString();
}

DebInstaller::DebInstaller(QObject *parent)
    : QObject(parent)
    , m_backend(new QApt::Backend(this))
    , m_debFile(nullptr)
    , m_transaction(nullptr)
    , m_status(DebInstaller::Begin)
{

}

QString DebInstaller::fileName() const
{
    return m_fileName;
}

void DebInstaller::setFileName(const QString &fileName)
{
    if (fileName.isEmpty() || !m_backend->init()
            || m_fileName == fileName)
        return;

    QString newPath = fileName;
    newPath = newPath.remove("file://");

    QFileInfo info(newPath);
    QString mimeType = QMimeDatabase().mimeTypeForFile(info.absoluteFilePath()).name();

    if (mimeType != "application/vnd.debian.binary-package")
        return;

    m_fileName = info.absoluteFilePath();

    QApt::FrontendCaps caps = (QApt::FrontendCaps)(QApt::DebconfCap);
    m_backend->setFrontendCaps(caps);
    m_debFile = new QApt::DebFile(m_fileName);

    m_isValid = m_debFile->isValid();
    emit isValidChanged();

    if (!m_isValid) {
        return;
    }

    m_packageName = m_debFile->packageName();
    emit packageNameChanged();

    m_version = m_debFile->version();
    emit versionChanged();

    m_maintainer = m_debFile->maintainer();
    emit maintainerChanged();

    m_description = m_debFile->longDescription();
    emit descriptionChanged();

    m_homePage = m_debFile->homepage();
    emit homePageChanged();

    m_installedSize = formatByteSize(m_debFile->installedSize() * 1024.0, 1);
    emit installedSizeChanged();

    // QStringList fileList = m_debFile->fileList();
    // std::sort(fileList.begin(), fileList.end());

    emit fileNameChanged();
}

QString DebInstaller::packageName() const
{
    return m_packageName;
}

QString DebInstaller::version() const
{
    return m_version;
}

QString DebInstaller::maintainer() const
{
    return m_maintainer;
}

QString DebInstaller::description() const
{
    return m_description;
}

bool DebInstaller::isValid() const
{
    return m_isValid;
}

QString DebInstaller::homePage() const
{
    return m_homePage;
}

QString DebInstaller::installedSize() const
{
    return m_installedSize;
}

void DebInstaller::install()
{
    if (m_backend->markedPackages().size()) {
        m_transaction = m_backend->commitChanges();
    } else {
        m_transaction = m_backend->installFile(*m_debFile);
    }

    setupTransaction();
    m_transaction->run();
}

void DebInstaller::setupTransaction()
{
    if (!m_transaction)
        return;

    m_transaction->setLocale(setlocale(LC_MESSAGES, 0));

    connect(m_transaction, &QApt::Transaction::statusChanged, this, &DebInstaller::transactionStatusChanged);
    connect(m_transaction, &QApt::Transaction::errorOccurred, this, &DebInstaller::errorOccurred);
    connect(m_transaction, &QApt::Transaction::statusDetailsChanged, this, &DebInstaller::statusDetailsChanged);
}

void DebInstaller::setStatus(DebInstaller::Status status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

void DebInstaller::transactionStatusChanged(QApt::TransactionStatus status)
{
    switch (status) {
    case QApt::SetupStatus:
    case QApt::WaitingStatus:
    case QApt::AuthenticationStatus:
        m_statusMessage = tr("Starting");
        emit statusMessageChanged();
        break;
    case QApt::WaitingMediumStatus:
    case QApt::WaitingLockStatus:
    case QApt::WaitingConfigFilePromptStatus:
        m_statusMessage = tr("Waiting");
        emit statusMessageChanged();
        emit requestSwitchToInstallPage();
        setStatus(DebInstaller::Installing);
        break;
    case QApt::RunningStatus:
        m_statusMessage = tr("Installing");
        emit statusMessageChanged();
        emit requestSwitchToInstallPage();
        setStatus(DebInstaller::Installing);
        break;
    case QApt::LoadingCacheStatus:
        m_statusMessage = tr("Loading Cache");
        emit statusMessageChanged();
        setStatus(DebInstaller::Installing);
        break;
    case QApt::DownloadingStatus:
        m_statusMessage = tr("Downloading Packages");
        emit statusMessageChanged();
        setStatus(DebInstaller::Installing);
        break;
    case QApt::CommittingStatus:
        break;
    case QApt::FinishedStatus:
        if (m_transaction->role() == QApt::CommitChangesRole) {
            delete m_transaction;
            m_transaction = m_backend->installFile(*m_debFile);
            setupTransaction();
            m_transaction->run();
        } else if (m_transaction->role() == QApt::InstallFileRole) {
            m_statusMessage = tr("Installation successful");
            emit statusMessageChanged();
            setStatus(DebInstaller::Succeeded);
        }
        break;
    default:
        break;
    }
}

void DebInstaller::errorOccurred(QApt::ErrorCode error)
{
    switch (error) {
        case QApt::InitError:
        case QApt::WrongArchError: {
            m_statusMessage = tr("Installation failed");
            emit statusMessageChanged();
            setStatus(DebInstaller::Error);
            break;
        }
        default:
            break;
    }
}

void DebInstaller::statusDetailsChanged(const QString &message)
{
    m_statusDetails = m_statusDetails + message;
    emit statusDetailsTextChanged();
}

QString DebInstaller::statusDetails() const
{
    return m_statusDetails;
}

DebInstaller::Status DebInstaller::status() const
{
    return m_status;
}

QString DebInstaller::statusMessage() const
{
    return m_statusMessage;
}
