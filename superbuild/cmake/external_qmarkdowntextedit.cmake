ExternalProject_Add(qmarkdowntextedit
        GIT_REPOSITORY "https://github.com/pbek/qmarkdowntextedit.git"
        GIT_TAG        "6dc71db95691361ff74222276d308f443dde5a69"  # b2137
        GIT_SHALLOW    ON
        BUILD_COMMAND cmake --build . --parallel
        INSTALL_COMMAND cmake --install .
        CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${SB_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DQMARKDOWNTEXTEDIT_EXE=OFF -DQT_VERSION_MAJOR=6
        PATCH_COMMAND  PATCH_COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/patches/qmarkdowntextedit/CMakeLists.txt" <SOURCE_DIR>/CMakeLists.txt
)