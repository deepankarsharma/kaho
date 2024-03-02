
cmake_minimum_required(VERSION 3.20)

project(kaho-superbuild)

#if (WIN32)
#    set(CMAKE_GENERATOR_PLATFORM x64)
#endif()

set(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_SOURCE_DIR}/../cmake
    ${CMAKE_MODULE_PATH}
)

include(ExternalProject)

set(SB_INSTALL_PREFIX "/opt/kaho/${CMAKE_BUILD_TYPE}")
if(NOT EXISTS "${SB_INSTALL_PREFIX}")
    file(MAKE_DIRECTORY "${SB_INSTALL_PREFIX}")
endif()

include(external_llama)
include(external_cmark_gfm)
include(external_qt)
include(external_qmarkdowntextedit)
#include(external_litehtml)
#include(external_qlitehtml)

if (APPLE)
    include(external_sparkle)    
endif ()
