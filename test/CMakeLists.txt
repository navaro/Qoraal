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

# Specify the linker script
set_target_properties(qoraal_test PROPERTIES LINK_FLAGS "-Wl,-Map=output.map -T ${CMAKE_SOURCE_DIR}/test/posix.ld")
