add_custom_target(python ALL
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bin/build_python.sh ${SB_INSTALL_PREFIX} ${CMAKE_BUILD_TYPE} 3.10.11
    COMMENT "Building python"
)

