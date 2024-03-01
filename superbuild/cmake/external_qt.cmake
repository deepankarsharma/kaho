if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE "-debug")
else()
    set(BUILD_TYPE "-release")
endif()

ExternalProject_Add(qt
  GIT_REPOSITORY "https://github.com/qt/qt5"
  GIT_TAG        "e6ff9950e1d0365d2c4f231fd1b6fe194144fd52"  # v6.6.1
  GIT_SHALLOW    ON
  GIT_SUBMODULES ""
  BUILD_IN_SOURCE ON
  CONFIGURE_COMMAND perl <SOURCE_DIR>/init-repository -f -module-subset=qtbase,qtsvg,qtimageformats && <SOURCE_DIR>/configure -static -prefix ${SB_INSTALL_PREFIX} -cmake-generator Ninja ${BUILD_TYPE} -nomake examples -nomake tests -qt-libpng -qt-zlib -qt-libjpeg -qt-freetype -qt-pcre -qt-harfbuzz -sql-sqlite -qt-sqlite -no-zstd
  BUILD_COMMAND cmake --build . --parallel -- -j8
  INSTALL_COMMAND cmake --install .
)
