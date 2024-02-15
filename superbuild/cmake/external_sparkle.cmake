if (WIN32)
    ExternalProject_Add(
            libwinsparkle
            URL https://github.com/vslavik/winsparkle/releases/download/v0.8.1/WinSparkle-0.8.1.zip
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND cmake -E echo "Skipping install step."
            LOG_BUILD ON
            SOURCE_DIR ${SB_INSTALL_PREFIX}/sparkle
    )
endif ()


if (APPLE)
    ExternalProject_Add(
            libsparkle
            URL https://github.com/sparkle-project/Sparkle/releases/download/2.5.2/Sparkle-2.5.2.tar.xz
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND cmake -E echo "Skipping install step."
            LOG_BUILD ON
            SOURCE_DIR ${SB_INSTALL_PREFIX}/sparkle
    )
endif ()


