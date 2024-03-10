ExternalProject_Add(llama
  GIT_REPOSITORY "https://github.com/ggerganov/llama.cpp"
  GIT_TAG        "29eee404746e4696143a4f3a642660a4793a15d8"  # b2343
  GIT_SHALLOW    ON
  BUILD_COMMAND cmake --build . --parallel
  INSTALL_COMMAND cmake --install .
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${SB_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=Release -DLLAMA_METAL_EMBED_LIBRARY=ON
)
