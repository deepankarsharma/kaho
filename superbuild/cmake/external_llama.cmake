ExternalProject_Add(llama
  GIT_REPOSITORY "https://github.com/ggerganov/llama.cpp"
  GIT_TAG        "49cc1f7d67de2da99f3ac185f9ff1319b7bf35f8"  # b2137
  GIT_SHALLOW    ON
  BUILD_COMMAND cmake --build . --parallel
  INSTALL_COMMAND cmake --install .
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${SB_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=Release
)