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

#ifndef DEBINSTALLER_H
#define DEBINSTALLER_H

#include <QObject>

// QApt
#include <QApt/DebFile>
#include <QApt/Globals>
#include <QApt/Package>
#include <QApt/Transaction>

class DebInstaller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString packageName READ packageName NOTIFY packageNameChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(QString maintainer READ maintainer NOTIFY maintainerChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString homePage READ homePage NOTIFY homePageChanged)
    Q_PROPERTY(QString installedSize READ installedSize NOTIFY installedSizeChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString statusDetails READ statusDetails NOTIFY statusDetailsTextChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY isValidChanged)

public:
    enum Status {
        Begin = 0,
        Installing,
        Error,
        Succeeded,
    };
    Q_ENUM(Status);

    explicit DebInstaller(QObject *parent = nullptr);

    QString fileName() const;
    void setFileName(const QString &fileName);

    QString packageName() const;
    QString version() const;
    QString maintainer() const;
    QString description() const;

    bool isValid() const;

    QString homePage() const;
    QString installedSize() const;

    Q_INVOKABLE void install();

    QString statusMessage() const;
    QString statusDetails() const;

    Status status() const;

signals:
    void fileNameChanged();
    void packageNameChanged();
    void versionChanged();
    void maintainerChanged();
    void descriptionChanged();
    void isValidChanged();
    void homePageChanged();
    void installedSizeChanged();
    void statusMessageChanged();
    void statusDetailsTextChanged();
    void statusChanged();

    void requestSwitchToInstallPage();

private:
    void setupTransaction();
    void setStatus(Status status);

private slots:
    void transactionStatusChanged(QApt::TransactionStatus status);
    void errorOccurred(QApt::ErrorCode error);
    void statusDetailsChanged(const QString &message);

private:
    QApt::Backend *m_backend;
    QApt::DebFile *m_debFile;
    QApt::Transaction *m_transaction;

    bool m_isValid;

    QString m_fileName;
    QString m_packageName;
    QString m_version;
    QString m_maintainer;
    QString m_description;
    QString m_homePage;
    QString m_installedSize;

    QString m_statusMessage;
    QString m_statusDetails;

    Status m_status;
};

#endif // DEBINSTALLER_H
