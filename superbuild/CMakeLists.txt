cmake_minimum_required(VERSION 3.25)

project(kaho-superbuild)

set(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_SOURCE_DIR}/../cmake
    ${CMAKE_MODULE_PATH}
)

include(ExternalProject)
include(superbuild)

set(SB_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/third_party")

include(external_qt)
include(external_llama)
include(external_sparkle)