
cmake_minimum_required(VERSION 3.25)

project(kaho-superbuild)

set(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_SOURCE_DIR}/../cmake
    ${CMAKE_MODULE_PATH}
)

include(ExternalProject)

set(SB_INSTALL_PREFIX "/opt/kaho/${CMAKE_BUILD_TYPE}")

include(external_qt)
include(external_llama)

if (APPLE)
    include(external_sparkle)    
endif ()
