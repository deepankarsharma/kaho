include(FindPackageHandleStandardArgs)

find_path(SPARKLE_INCLUDE_DIR Sparkle.h
    PATHS ${KAHO_SB_INSTALL_PREFIX}/sparkle/Sparkle.framework/Versions/B/Headers
)

find_library(SPARKLE_LIBRARY 
    NAMES Sparkle
    PATHS ${KAHO_SB_INSTALL_PREFIX}/sparkle
)

find_package_handle_standard_args(Sparkle DEFAULT_MSG SPARKLE_INCLUDE_DIR SPARKLE_LIBRARY)
mark_as_advanced(SPARKLE_INCLUDE_DIR SPARKLE_LIBRARY)