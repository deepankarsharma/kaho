include(FindPackageHandleStandardArgs)

find_path(QLITEHTML_INCLUDE_DIR qlitehtmlwidget.h
        PATHS ${KAHO_SB_INSTALL_PREFIX}/include/qlitehtml
)

find_library(QLITEHTML_LIBRARY
        NAMES qlitehtml
        PATHS ${KAHO_SB_INSTALL_PREFIX}/lib
)

find_package_handle_standard_args(qlitehtml DEFAULT_MSG QLITEHTML_INCLUDE_DIR QLITEHTML_LIBRARY)
mark_as_advanced(QLITEHTML_INCLUDE_DIR QLITEHTML_LIBRARY)