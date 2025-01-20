# Collect all test source files
set(TEST_SRCS
    main.c
    demo.c
    shell.c
    system.c
)

# Add the executable target
add_executable(qoraal_test ${TEST_SRCS})

# Link the qoraal library and pthread
target_link_libraries(qoraal_test qoraal pthread)
target_compile_options(qoraal PRIVATE -O0 -g)
target_compile_options(qoraal_test PRIVATE -O0 -g)

# Generate a map file
set_target_properties(qoraal_test PROPERTIES LINK_FLAGS "-Wl,-Map=output.map -T ${CMAKE_SOURCE_DIR}/test/posix.ld")