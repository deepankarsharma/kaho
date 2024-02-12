ExternalProject_Add(qt
  GIT_REPOSITORY "https://github.com/qt/qt5"
  GIT_TAG        "e6ff9950e1d0365d2c4f231fd1b6fe194144fd52"  # v6.6.1
  CONFIGURE_COMMAND perl <SOURCE_DIR>/init-repository && <SOURCE_DIR>/configure -prefix ${SB_INSTALL_PREFIX} -cmake-generator Ninja -static -release -nomake examples -nomake tests -qt-libpng -qt-zlib -qt-libjpeg -qt-freetype -qt-pcre -qt-harfbuzz -platform macx-clang -sql-sqlite -qt-sqlite
  BUILD_COMMAND cmake --build . --parallel
  INSTALL_COMMAND cmake --install .
)