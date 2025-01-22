# Collect all test source files
set(TEST_SRCS
    main.c
    services/demo.c
    services/shell.c
    services/system.c
    platform/platform.c
)

# Add the executable target
add_executable(qoraal_test ${TEST_SRCS})

# Link the qoraal library and pthread
target_link_libraries(qoraal_test qoraal pthread)
target_compile_options(qoraal PRIVATE -O0 -g)
target_compile_options(qoraal_test PRIVATE -O0 -g)

target_link_options(qoraal_test PRIVATE
    "-Wl,--whole-archive"
    "$<TARGET_FILE:qoraal>"
    "-Wl,--no-whole-archive"
)

# Generate a map file
set_target_properties(qoraal_test PROPERTIES LINK_FLAGS "-Wl,-Map=output.map -T ${CMAKE_SOURCE_DIR}/test/platform/posix.ld")