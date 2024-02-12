ExternalProject_Add(qt
  GIT_REPOSITORY "https://github.com/qt/qtbase.git"
  GIT_TAG        "e2cbce919ccefcae2b18f90257d67bc6e24c3c94"  # v6.6.1
  CONFIGURE_COMMAND <SOURCE_DIR>/configure -prefix ${SB_INSTALL_PREFIX} -cmake-generator Ninja -static -release -nomake examples -nomake tests -qt-libpng -qt-zlib -qt-libjpeg -qt-freetype -qt-pcre -qt-harfbuzz -platform macx-clang -sql-sqlite -qt-sqlite
  BUILD_COMMAND cmake --build . --parallel
  INSTALL_COMMAND cmake --install .
)