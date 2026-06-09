cmake_minimum_required(VERSION 3.16)

project(Command_Workstation VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(appCommand_Workstation
    main.cpp
)

qt_add_qml_module(appCommand_Workstation
    URI Command_Workstation
    QML_FILES
        Main.qml
    SOURCES
        src/database_manager.h
        src/database_manager.cpp
        src/command_runner.h
        src/command_runner.cpp
        src/workstation_bridge.h
        src/workstation_bridge.cpp
        src/terminal_launcher.h
        src/terminal_launcher.cpp
)

set_target_properties(appCommand_Workstation PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appCommand_Workstation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(appCommand_Workstation
    PRIVATE Qt6::Quick Qt6::Sql
)

install(TARGETS appCommand_Workstation
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
