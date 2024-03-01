cmake_minimum_required(VERSION 3.20)

# Make sure you bump version in bin/base.sh when you change this
# For more details refer to bumping_versions playbook
project(kaho VERSION 8.1.0 LANGUAGES CXX)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE} CMAKE_C_COMPILER: ${CMAKE_C_COMPILER} CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")


############## Configure cmake ######################
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Enabling sanitizers for debug build...")
        add_compile_options($<$<CONFIG:Debug>:-fsanitize=address,undefined>)
        add_link_options($<$<CONFIG:Debug>:-fsanitize=address,undefined>)
    endif()
endif()




############## Kaho specific configuration ##########
set(KAHO_SB_INSTALL_PREFIX "/opt/kaho/${CMAKE_BUILD_TYPE}")

option(KAHO_MAC_SIGN_APP "Option for signing the Mac app" OFF)

if (APPLE)
    set(QT_DEFAULT_DIR "$ENV{HOME}/Qt/6.6.1/macos")
elseif(LINUX)
    set(QT_DEFAULT_DIR "$ENV{HOME}/Qt/6.6.1")
elseif(WIN32)
    set(QT_DEFAULT_DIR "c:/Qt")
else()
    MESSAGE(ERROR "Unknown platform")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH ${QT_DEFAULT_DIR})
list(APPEND CMAKE_PREFIX_PATH ${KAHO_SB_INSTALL_PREFIX})



############# Dependencies ############################
if (APPLE)
    list(APPEND CMAKE_PREFIX_PATH "${KAHO_SB_INSTALL_PREFIX}")
    find_package(Sparkle REQUIRED)
endif()

include(Dependencies)

message(STATUS "Looking for cmark")
include(FindCmark)


if (APPLE)
    message(STATUS "Looking for sparkle")
    include(FindSparkle)
endif()

include(FindQLiteHtml)
find_package(qlitehtml REQUIRED)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Network Core Svg)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network Core Svg)




############ Kaho static library ######################
qt_standard_project_setup()

set(LITEHTML_BUILD_TESTING OFF)
add_subdirectory(external/litehtml)

set(PROJECT_SOURCES
        include/kaho/main.h
        src/main.cpp
)

if(WIN32)
    list(APPEND PROJECT_SOURCES
            src/winsparkle_auto_updater.cpp
            include/kaho/updater.h
    )
endif()

if(APPLE)
    list(APPEND PROJECT_SOURCES
            src/sparkle_auto_updater.mm
            include/kaho/updater.h
            ${KAHO_ICON}
    )
endif()


set(KAHO_LIBRARIES Qt6::Widgets Qt6::Network Qt6::Core cmark::cmark cmark::extensions ${QLITEHTML_LIBRARY} litehtml gumbo)
set(KAHO_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/external/cmark-gfm/src")

if (APPLE)
    list(APPEND KAHO_LIBRARIES ${SPARKLE_LIBRARY})
    list(APPEND KAHO_INCLUDE_DIRS ${SPARKLE_INCLUDE_DIR})
endif()

set(OSX_FRAMEWORKS "-framework AppKit -framework Carbon -framework Foundation -framework ApplicationServices -framework Sparkle")

add_library(kaho_static STATIC ${PROJECT_SOURCES})
target_link_libraries(kaho_static PRIVATE ${KAHO_LIBRARIES})
target_include_directories(kaho_static PUBLIC ${KAHO_INCLUDE_DIRS})


set(resource_files
        "images/c1.png"
        "images/c2.png"
        "images/icons8-search.svg"
        "images/icons8-home.svg"
        "images/icons8-server.svg"
        "images/icons8-settings.svg"
        "images/icons8-chat.svg"
        "images/icons8-enter.svg"
        "images/icons8-send.svg"
        "media/edit-find-replace.svg"
        "media/go-bottom.svg"
        "media/window-close.svg"
        "media/format-text-superscript.svg"
        "media/go-top.svg"
)

qt_add_resources(kaho_static "kaho_static"
        PREFIX
        "/"
        FILES
        ${resource_files}
)

##################### Kaho private exe ######################

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE logo.icns)
    set(KAHO_ICON ${CMAKE_CURRENT_SOURCE_DIR}/art/mac_installer/logo.icns)
    set_source_files_properties(${KAHO_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

endif()


qt_add_executable(kaho
        MANUAL_FINALIZATION
        src/public_main.cpp
        include/kaho/main.h
)
target_include_directories(kaho PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(kaho PRIVATE kaho_static)

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

# if (LINUX)
#     qt_import_plugins(kaho INCLUDE Qt::QWaylandIntegrationPlugin)
# endif()

include(GNUInstallDirs)
install(TARGETS kaho
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(kaho)


if (WIN32)
    set(SERVER_EXE "server.exe")
else()
    set(SERVER_EXE "server")
endif()

add_custom_target(copy_server ALL
        COMMAND cmake -E copy ${KAHO_SB_INSTALL_PREFIX}/bin/${SERVER_EXE} $<TARGET_FILE_DIR:kaho>
        COMMAND cmake -E copy ${KAHO_SB_INSTALL_PREFIX}/bin/ggml-metal.metal $<TARGET_FILE_DIR:kaho>
        COMMENT "Copying server to output directory"
)
add_dependencies(copy_server kaho)


if (APPLE)
    set(CLEAN_UP_FILES "kaho.app")
    set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "${CLEAN_UP_FILES}")

    add_custom_target(sparkle_framework ALL
            COMMAND mkdir -p "$<TARGET_FILE_DIR:kaho>/../Frameworks/Sparkle.framework" && cp -a ${KAHO_SB_INSTALL_PREFIX}/sparkle/Sparkle.framework $<TARGET_FILE_DIR:kaho>/../Frameworks
            COMMENT "Copying Sparkle.framework to output directory"
    )
    add_dependencies(sparkle_framework copy_server)


#    add_custom_target(macdeployqt ALL
#            COMMAND "${QT_DEFAULT_DIR}/bin/macdeployqt" kaho.app
#            COMMENT "Running macdeployqt"
#    )
#    add_dependencies(macdeployqt sparkle_framework)

#    if (KAHO_MAC_SIGN_APP)
#        message(STATUS "==> Code signing")
#        add_custom_target(codesign ALL VERBATIM
#                COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bin/mac_codesign.sh
#                COMMENT "Codesign kaho.app"
#        )
#        add_dependencies(codesign macdeployqt)
#    endif()
    set_target_properties(kaho PROPERTIES LINK_FLAGS "-F/Library/Frameworks -Wl -rpath @loader_path/../Frameworks")
endif ()
