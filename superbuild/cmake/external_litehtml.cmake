ExternalProject_Add(llama
        GIT_REPOSITORY "https://github.com/litehtml/litehtml.git"
        GIT_TAG        "289c153aa6ffe68dc0fd2199bca7d0229d652908"
        GIT_SHALLOW    ON
        BUILD_COMMAND cmake --build . --parallel
        INSTALL_COMMAND cmake --install .
        CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${SB_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DLITEHTML_BUILD_TESTING=OFF
)