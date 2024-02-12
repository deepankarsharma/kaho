cmake_minimum_required(VERSION 3.5)

project(kaho VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(KAHO_SB_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/superbuild/third_party")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${KAHO_SB_INSTALL_PREFIX}")


find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Network Core Svg)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network Core Svg)
qt_standard_project_setup()

set(PROJECT_SOURCES
    src/main.cpp
    src/aimodel.cpp
    src/chatview.cpp
    src/file_utils.cpp
    src/mainwindow.cpp
    src/noneditablestringlistmodel.cpp
    src/promptedit.cpp
    src/request.cpp
    include/kaho/aimodel.h
    include/kaho/chatview.h
    include/kaho/mainwindow.h
    include/kaho/promptedit.h
    include/kaho/base.h
    include/kaho/file_utils.h
    include/kaho/noneditablestringlistmodel.h
)

qt_add_executable(kaho
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)
target_link_libraries(kaho PRIVATE Qt6::Widgets Qt6::Network Qt6::Core)
target_include_directories(kaho PUBLIC "${PROJECT_SOURCE_DIR}/include")

set_target_properties(kaho PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS kaho
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set(resource_files
    "images/c1.png"
    "images/c2.png"
    "images/icons8-search.svg"
    "images/icons8-home.svg"
    "images/icons8-server.svg"
    "images/icons8-settings.svg"
    "images/icons8-chat.svg"
)

qt_add_resources(kaho "kaho"
    PREFIX
        "/"
    FILES
        ${resource_files}
)
qt_finalize_executable(kaho)
