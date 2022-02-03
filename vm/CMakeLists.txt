cmake_minimum_required(VERSION 3.8)

set(
    SOURCE
    main.c
    defs.h
    bfm.h
    bfm.c
    bfe.h
    bfe.c
)

add_executable(bfm ${SOURCE})
set_target_properties(
    bfm PROPERTIES
    C_STANDARD                  11
    C_STANDARD_REQUIRED         1
    RUNTIME_OUTPUT_DIRECTORY    "${PROJECT_SOURCE_DIR}/bin"
)
