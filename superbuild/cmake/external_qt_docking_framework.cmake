

ExternalProject_Add(qtadvanceddockingframework
        GIT_REPOSITORY "https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System.git"
        GIT_TAG        "8da0713bf0d964f0c5b62e87a8aa61bcae3fa212"
        GIT_SHALLOW    ON
        BUILD_COMMAND cmake --build . --parallel
        INSTALL_COMMAND cmake --install .
        CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${SB_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DQT_VERSION_MAJOR=6 -DBUILD_STATIC=ON -DBUILD_EXAMPLES=OFF
)
