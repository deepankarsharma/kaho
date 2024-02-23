cmake_minimum_required(VERSION 3.5)

# Make sure you bump version in bin/base.sh when you change this
# For more details refer to bumping_versions playbook
project(kaho VERSION 0.3.3 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
set(KAHO_SB_INSTALL_PREFIX "/opt/kaho/${CMAKE_BUILD_TYPE}")
set(KAHO_DEPLOYMENT_BUILD OFF)
option(KAHO_MAC_SIGN_APP "Option for signing the Mac app" OFF)
set(KAHO_MAC_BUILD_DMG OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (APPLE)
    list(APPEND CMAKE_PREFIX_PATH "${KAHO_SB_INSTALL_PREFIX}")
    find_package(Sparkle REQUIRED)
endif()

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Network Core Svg Wayland)
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
    src/server.cpp
    src/downloader.cpp
        src/errors.cpp
        src/localmodelregistry.cpp
    include/kaho/aimodel.h
    include/kaho/chatview.h
    include/kaho/mainwindow.h
    include/kaho/promptedit.h
    include/kaho/base.h
    include/kaho/file_utils.h
    include/kaho/noneditablestringlistmodel.h
    include/kaho/server.h
    include/kaho/downloader.h
        include/kaho/errors.h
        include/kaho/localmodelregistry.h

)

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE logo.icns)
    set(KAHO_ICON ${CMAKE_CURRENT_SOURCE_DIR}/art/mac_installer/logo.icns)
    set_source_files_properties(${KAHO_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    list(APPEND PROJECT_SOURCES
        external/updater/SparkleAutoUpdater.mm
        external/updater/CocoaInitializer.mm
        external/updater/AutoUpdater.cpp
        external/updater/SparkleAutoUpdater.h
        external/updater/CocoaInitializer.h
        external/updater/AutoUpdater.h
            ${KAHO_ICON}
    )
endif()

qt_add_executable(kaho
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

set(KAHO_LIBRARIES Qt6::Widgets Qt6::Network Qt6::Core)
set(KAHO_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/external")

if (APPLE)
    list(APPEND KAHO_LIBRARIES ${SPARKLE_LIBRARY})
    list(APPEND KAHO_INCLUDE_DIRS ${SPARKLE_INCLUDE_DIR})
endif()


target_link_libraries(kaho PRIVATE ${KAHO_LIBRARIES})
target_include_directories(kaho PUBLIC ${KAHO_INCLUDE_DIRS})

string(TIMESTAMP CURRENT_YEAR "%Y")

set_target_properties(kaho PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_NAME "Kaho"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_COPYRIGHT "Copyright Deepankar Sharma 2024-${CURRENT_YEAR}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "io.kaho.kaho"
    MACOSX_BUNDLE_ICON_FILE "logo"
    MACOSX_BUNDLE_ICON_NAME "logo"
    MACOSX_BUNDLE_INFO_STRING "Kaho"
    MACOSX_SPARKLE_UPDATE_PUBLIC_KEY "p9cmJiv4q1h2CGpArGVFUOw6AJw9+tfL3MMcvo4ut+A="
    MACOSX_SPARKLE_UPDATE_FEED_URL   "https://mac-updates.dz9.org/appcast.xml"

    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if (LINUX)
    qt_import_plugins(kaho INCLUDE Qt::QWaylandIntegrationPlugin)
endif()

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


if (APPLE)
    set(CLEAN_UP_FILES "kaho.app")
    set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "${CLEAN_UP_FILES}")

    set(OSX_FRAMEWORKS "-framework AppKit -framework Carbon -framework Foundation -framework ApplicationServices -framework Sparkle")

    add_custom_target(sparkle_framework ALL
            COMMAND mkdir -p "$<TARGET_FILE_DIR:kaho>/../Frameworks/Sparkle.framework" && cp -a ${KAHO_SB_INSTALL_PREFIX}/sparkle/Sparkle.framework $<TARGET_FILE_DIR:kaho>/../Frameworks
            COMMENT "Copying Sparkle.framework to output directory"
    )
    add_dependencies(sparkle_framework kaho)

    add_custom_target(copy_server ALL
            COMMAND cp ${KAHO_SB_INSTALL_PREFIX}/bin/server $<TARGET_FILE_DIR:kaho> && cp ${KAHO_SB_INSTALL_PREFIX}/bin/ggml-metal.metal $<TARGET_FILE_DIR:kaho>
            COMMENT "Copying server to output directory"
    )
    add_dependencies(copy_server kaho)

    if (KAHO_MAC_SIGN_APP)
        message(STATUS "==> Code signing")
        add_custom_target(codesign ALL VERBATIM
                COMMAND codesign --deep --force --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app
                COMMENT "Codesign kaho.app"
        )
        add_dependencies(codesign sparkle_framework)
    endif()
#    if(KAHO_BUILD_DMG)
#        if(KAHO_DEPLOYMENT_BUILD)
#            set(NOTARIZE_FLAG "--notarize")
#        else()
#            set(NOTARIZE_FLAG "")
#        endif()
#        set(KAHO_INSTALLER_ART_DIR )
#        message(STATUS "==> Building DMG volicon => ${CMAKE_CURRENT_SOURCE_DIR}/art/mac_installer/logo.icns")
#
#        add_custom_target(create_dmg ALL VERBATIM
#                COMMAND create-dmg --codesign "Apple Distribution: Deepankar Sharma (VTN5FB5JF7)"
#                    ${NOTARIZE_FLAG}
#                    --volname "Kaho Installer"
#                    --volicon ${CMAKE_CURRENT_SOURCE_DIR}/art/mac_installer/logo.icns
#                    --background ${CMAKE_CURRENT_SOURCE_DIR}/art/mac_installer/background.png
#                    --window-pos 200 120
#                    --window-size 800 400
#                    --icon-size 100
#                    --icon "kaho.app" 200 190
#                    --hide-extension "kaho.app"
#                    --app-drop-link 600 185
#                    kaho.dmg kaho.app
#                COMMENT "Create DMG"
#        )
#        add_dependencies(create_dmg codesign)
#    endif()
set_target_properties(kaho PROPERTIES LINK_FLAGS "-F/Library/Frameworks -Wl -rpath @loader_path/../Frameworks")
endif ()
