include(FindPackageHandleStandardArgs)

find_path(QMARKDOWNTEXTEDIT_INCLUDE_DIR qmarkdowntextedit.h
        PATHS ${KAHO_SB_INSTALL_PREFIX}/include
)

find_library(QMARKDOWNTEXTEDIT_LIBRARY
        NAMES qmarkdowntextedit
        PATHS ${KAHO_SB_INSTALL_PREFIX}/lib
)

find_package_handle_standard_args(qmarkdowntextedit DEFAULT_MSG QMARKDOWNTEXTEDIT_INCLUDE_DIR QMARKDOWNTEXTEDIT_LIBRARY)
mark_as_advanced(QMARKDOWNTEXTEDIT_INCLUDE_DIR QMARKDOWNTEXTEDIT_LIBRARY)