if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE "-debug")
else()
    set(BUILD_TYPE "-release")
endif()

set(QPA_FLAG "")
set(WAYLAND_MODULE "")
set(WAYLAND_FLAG "")
if(LINUX)
    set(QPA_FLAG "-qpa wayland,xcb")
    set(WAYLAND_MODULE "qtwayland")
endif()


ExternalProject_Add(qt
  GIT_REPOSITORY "https://github.com/qt/qt5"
  GIT_TAG        "e6ff9950e1d0365d2c4f231fd1b6fe194144fd52"  # v6.6.1
  GIT_SHALLOW    ON
  GIT_SUBMODULES ""
  BUILD_IN_SOURCE ON
  CONFIGURE_COMMAND perl <SOURCE_DIR>/init-repository -f -module-subset=qtbase,qtsvg,qtimageformats,${WAYLAND_MODULE} && <SOURCE_DIR>/configure -static -prefix ${SB_INSTALL_PREFIX} -cmake-generator Ninja ${BUILD_TYPE} -nomake examples -nomake tests -qt-libpng -qt-zlib -qt-libjpeg -qt-freetype -qt-pcre -qt-harfbuzz -sql-sqlite -qt-sqlite -no-zstd ${WAYLAND_FLAG}}
  BUILD_COMMAND cmake --build . --parallel
  INSTALL_COMMAND cmake --install .
)
