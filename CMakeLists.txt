cmake_minimum_required(VERSION 3.10)
project(qoraal C)

# Set the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add compile definitions globally
add_compile_definitions(CFG_OS_POSIX)

# Include directories
include_directories(include)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(test)
