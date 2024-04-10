ExternalProject_Add(qmarkdowntextedit
        GIT_REPOSITORY "https://github.com/pbek/qmarkdowntextedit.git"
        GIT_TAG        "e30c97859b2c208d76d7f520d51f47e7cc8a67e2"
        GIT_SHALLOW    ON
        BUILD_COMMAND cmake --build . --parallel
        INSTALL_COMMAND cmake --install .
        CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${SB_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DQMARKDOWNTEXTEDIT_EXE=OFF -DQT_VERSION_MAJOR=6
        PATCH_COMMAND  PATCH_COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/patches/qmarkdowntextedit/CMakeLists.txt" <SOURCE_DIR>/CMakeLists.txt
)
