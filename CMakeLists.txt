cmake_minimum_required(VERSION 3.20)

# Make sure you bump version in bin/base.sh when you change this
# For more details refer to bumping_versions playbook
project(kaho VERSION 1.4.0 LANGUAGES CXX)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE} CMAKE_C_COMPILER: ${CMAKE_C_COMPILER} CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")


############## Configure cmake ######################
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(KAHO_PYTHON_VERSION "3.10.11")
#IF(APPLE)
#    SET(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)
#ENDIF(APPLE)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Enabling sanitizers for debug build...")
        add_compile_options($<$<CONFIG:Debug>:-fsanitize=address,undefined>)
        add_link_options($<$<CONFIG:Debug>:-fsanitize=address,undefined>)
    endif()
endif()




############## Kaho specific configuration ##########
set(KAHO_SB_INSTALL_PREFIX "/opt/kaho/${CMAKE_BUILD_TYPE}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH ${KAHO_SB_INSTALL_PREFIX})



############# Dependencies ############################
if (APPLE)
    list(APPEND CMAKE_PREFIX_PATH "${KAHO_SB_INSTALL_PREFIX}")
    find_package(Sparkle REQUIRED)
endif()

include(Dependencies)

message(STATUS "Looking for qmarkdowntextedit")
include(Findqmarkdowntextedit)
find_package(qmarkdowntextedit REQUIRED)

if (APPLE)
    message(STATUS "Looking for sparkle")
    include(FindSparkle)
endif()


find_package(Qt6 REQUIRED COMPONENTS Widgets Network Core Svg Sql)




############ Kaho static library ######################
qt_standard_project_setup()


set(PROJECT_SOURCES
        include/kaho/updater.h
        include/kaho/main.h
        src/main.cpp
)

if(WIN32)
    list(APPEND PROJECT_SOURCES
            src/winsparkle_auto_updater.cpp
    )
endif()

if(APPLE)
    list(APPEND PROJECT_SOURCES
            src/sparkle_auto_updater.mm
            ${KAHO_ICON}
    )
endif()

if(LINUX)
    list(APPEND PROJECT_SOURCES
            src/linux_auto_updater.cpp
    )
endif()


# cmark::cmark cmark::extensions
set(KAHO_LIBRARIES Qt6::Widgets Qt6::Network Qt6::Core Qt6::Sql ${QMARKDOWNTEXTEDIT_LIBRARY})
set(KAHO_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" ${QMARKDOWNTEXTEDIT_INCLUDE_DIR})

if (APPLE)
    list(APPEND KAHO_LIBRARIES ${SPARKLE_LIBRARY})
    list(APPEND KAHO_INCLUDE_DIRS ${SPARKLE_INCLUDE_DIR})
endif()

if (LINUX)
    # Qt6::FontDatabaseSupport
    # Qt6::XcbQpa
    list(APPEND KAHO_LIBRARIES Qt6::QWaylandIntegrationPlugin)
endif()

if(APPLE)
    list(APPEND KAHO_LIBRARIES Qt6::QCocoaIntegrationPlugin)
    #qt_import_plugins(kaho INCLUDE Qt::QCocoaIntegrationPlugin)
endif()

set(OSX_FRAMEWORKS "-framework AppKit -framework Carbon -framework Foundation -framework ApplicationServices -framework Sparkle")

add_library(kaho_static STATIC ${PROJECT_SOURCES})
target_link_libraries(kaho_static PRIVATE ${KAHO_LIBRARIES})
target_include_directories(kaho_static PUBLIC ${KAHO_INCLUDE_DIRS})

if (UNIX)
    message(STATUS "Marking kaho_static library for stripping in release build.")
    set_target_properties(kaho_static PROPERTIES LINK_FLAGS_RELEASE -s)
endif()

##################### Kaho private exe ######################

set(PROJECT_SOURCES
        src/public_main.cpp
        include/kaho/main.h
)

if(APPLE)
    set(KAHO_ICON ${CMAKE_CURRENT_SOURCE_DIR}/art/mac_installer/logo.icns)
    set_source_files_properties(${KAHO_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    list(APPEND PROJECT_SOURCES
            ${KAHO_ICON}
    )
endif()

qt_add_executable(kaho
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        superbuild/cmake/external_python.cmake

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
    MACOSX_SPARKLE_UPDATE_FEED_URL   "https://mac-updates.kaho.ai/appcast.xml"

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
        "images/icons8-upgrade.svg"
        "images/icons8-console.svg"
        "media/edit-find-replace.svg"
        "media/go-bottom.svg"
        "media/window-close.svg"
        "media/format-text-superscript.svg"
        "media/go-top.svg"
        "media/fonts/Cantarell-Bold.ttf"
        "media/fonts/Cantarell-BoldItalic.ttf"
        "media/fonts/Cantarell-Italic.ttf"
        "media/fonts/Cantarell-Regular.ttf"
        "media/fonts/Ubuntu-Bold.ttf"
        "media/fonts/Ubuntu-BoldItalic.ttf"
        "media/fonts/Ubuntu-Italic.ttf"
        "media/fonts/Ubuntu-Light.ttf"
        "media/fonts/Ubuntu-LightItalic.ttf"
        "media/fonts/Ubuntu-Medium.ttf"
        "media/fonts/Ubuntu-MediumItalic.ttf"
        "media/fonts/Ubuntu-Regular.ttf"
        "media/fonts/UbuntuCondensed-Regular.ttf"
        "media/fonts/UbuntuMono-Bold.ttf"
        "media/fonts/UbuntuMono-BoldItalic.ttf"
        "media/fonts/UbuntuMono-Italic.ttf"
        "media/fonts/UbuntuMono-Regular.ttf"
)

qt_add_resources(kaho "kaho"
        PREFIX
        "/"
        FILES
        ${resource_files}
)
qt_finalize_executable(kaho)


if (WIN32)
    set(SERVER_EXE "server.exe")
else()
    set(SERVER_EXE "server")
endif()

get_target_property(KAHO_TARGET_FILE_DIR kaho TARGET_FILE_DIR)

set(SERVER_DIR "<UNDEFINED>")
set(PYTHON_CODE_DIR "<UNDEFINED>")
if(APPLE)
    set(SERVER_DIR "${CMAKE_BINARY_DIR}/kaho.app/Contents/MacOS/server")
    set(KAHO_PYTHON_CODE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/python")
    set(KAHO_PYTHON_CODE_TARGET_DIR "${CMAKE_BINARY_DIR}/kaho.app/Contents/Resources")
endif()

#add_custom_target(copy_python ALL
#        COMMAND cmake -E copy_directory ${KAHO_PYTHON_CODE_SOURCE_DIR}/ ${KAHO_PYTHON_CODE_TARGET_DIR}
#        COMMENT "Copying python code to output directory"
#)
#add_dependencies(copy_python kaho)

add_custom_target(copy_server ALL
        COMMAND cmake -E copy ${KAHO_SB_INSTALL_PREFIX}/bin/${SERVER_EXE} $<TARGET_FILE_DIR:kaho>
        COMMENT "Copying server to output directory"
)
add_dependencies(copy_server kaho)

add_executable(embed src/embed.cpp)
target_link_libraries(embed PRIVATE ${KAHO_LIBRARIES})
target_include_directories(embed PUBLIC ${KAHO_INCLUDE_DIRS})

if (APPLE)
    set(CLEAN_UP_FILES "kaho.app")
    set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "${CLEAN_UP_FILES}")
    set_target_properties(kaho PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/mac/Info.plist")


    add_custom_target(sparkle_framework ALL
            COMMAND mkdir -p "$<TARGET_FILE_DIR:kaho>/../Frameworks/Sparkle.framework" && cp -a ${KAHO_SB_INSTALL_PREFIX}/sparkle/Sparkle.framework $<TARGET_FILE_DIR:kaho>/../Frameworks
            COMMENT "Copying Sparkle.framework to output directory"
    )
    add_dependencies(sparkle_framework copy_server)


#    add_custom_target(python_framework ALL
#            COMMAND mkdir -p "$<TARGET_FILE_DIR:kaho>/../Frameworks" && cp -a ${KAHO_SB_INSTALL_PREFIX}/python/Python.framework $<TARGET_FILE_DIR:kaho>/../Frameworks
#            COMMENT "Copying Python.framework to output directory"
#    )
#    add_dependencies(python_framework sparkle_framework)

    message(STATUS "==> Code signing")
    add_custom_target(codesign ALL VERBATIM
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bin/mac_codesign.sh
            COMMENT "Codesign kaho.app"
    )
    add_dependencies(codesign sparkle_framework)
    set_target_properties(kaho PROPERTIES LINK_FLAGS "-F/Library/Frameworks -Wl -rpath @loader_path/../Frameworks")
endif ()
