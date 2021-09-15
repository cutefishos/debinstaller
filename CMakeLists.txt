cmake_minimum_required(VERSION 3.14)

project(cutefish-debinstaller LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Widgets Quick LinguistTools REQUIRED)
find_package(QApt REQUIRED)

set(PROJECT_SOURCES
        src/main.cpp
        src/debinstaller.cpp
        qml.qrc
)

add_executable(cutefish-debinstaller
  ${PROJECT_SOURCES}
)

target_link_libraries(cutefish-debinstaller
  PRIVATE
  Qt5::Core
  Qt5::Widgets
  Qt5::Quick
  QApt
)

file(GLOB TS_FILES translations/*.ts)
qt5_create_translation(QM_FILES ${TS_FILES})
add_custom_target(translations DEPENDS ${QM_FILES} SOURCES ${TS_FILES})
add_dependencies(cutefish-debinstaller translations)

install(FILES ${QM_FILES} DESTINATION /usr/share/cutefish-debinstaller/translations)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES
    cutefish-debinstaller.desktop
    DESTINATION /usr/share/applications/
    COMPONENT Runtime
)
