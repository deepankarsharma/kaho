include(FindPackageHandleStandardArgs)

find_path(CMARK_INCLUDE_DIR cmark-gfm.h
        PATHS ${KAHO_SB_INSTALL_PREFIX}/
)

find_library(CMARK_LIBRARY
        NAMES cmark-gfm cmark-gfm_static
        PATHS ${KAHO_SB_INSTALL_PREFIX}/
)

find_library(CMARK_EXTENSIONS_LIBRARY
        NAMES cmark-gfm-extensions cmark-gfm-extensions_static
        PATHS ${KAHO_SB_INSTALL_PREFIX}/
)


set(CMARK_INCLUDE_DIRS ${CMARK_INCLUDE_DIR})
set(CMARK_LIBRARIES ${CMARK_LIBRARY} ${CMARK_EXTENSIONS_LIBRARY})
set(CMARK_LIBRARY_DIRS ${CMARK_LIBRARY} ${CMARK_EXTENSIONS_LIBRARY})

add_library(cmark::cmark UNKNOWN IMPORTED)
set_target_properties(cmark::cmark PROPERTIES
        IMPORTED_LOCATION ${CMARK_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${CMARK_INCLUDE_DIR}
)

add_library(cmark::extensions UNKNOWN IMPORTED)
set_target_properties(cmark::extensions PROPERTIES
        IMPORTED_LOCATION ${CMARK_EXTENSIONS_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${CMARK_INCLUDE_DIR}
)

mark_as_advanced(
        CMARK_INCLUDE_DIR
        CMARK_LIBRARY
        CMARK_EXTENSIONS_LIBRARY
)