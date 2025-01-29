# Collect all test source files
set(TEST_SRCS
    main.c
    services/demo.c
    services/system.c
)


# set options for compiling the qoraal library
target_compile_options(qoraal PRIVATE -O0 -g)
target_compile_definitions(qoraal PRIVATE CFG_OS_POSIX)

# Add the executable target
add_executable(qoraal_test ${TEST_SRCS})
target_compile_options(qoraal_test PRIVATE -O0 -g)
# Link the qoraal library and pthread
target_link_libraries(qoraal_test qoraal pthread)


# Generate a map file
set_target_properties(qoraal_test PROPERTIES 
        LINK_FLAGS "-Wl,-Map=output.map -T ${CMAKE_SOURCE_DIR}/test/posix.ld"
)