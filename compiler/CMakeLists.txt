cmake_minimum_required(VERSION 3.8)

set(
    SOURCE
    main.c
    defs.h
    lexer.h
    lexer.c
    bfc.h
    bfc.c
)

add_executable(bfc ${SOURCE})
set_target_properties(
    bfc PROPERTIES
    C_STANDARD                  11
    C_STANDARD_REQUIRED         1
    RUNTIME_OUTPUT_DIRECTORY    "${PROJECT_SOURCE_DIR}/bin"
)
