cmake_minimum_required(VERSION 3.20)
project(adachess VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Prefer Clang for development
if(NOT CMAKE_CXX_COMPILER_ID)
    find_program(CLANG_CXX clang++)
    if(CLANG_CXX)
        set(CMAKE_CXX_COMPILER ${CLANG_CXX})
    endif()
endif()

# Enable modern C++ warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
elseif(MSVC)
    add_compile_options(/W4 /WX /permissive-)
endif()

# Sanitizer support: Enabled by default in Debug builds
option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" ON)
if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        message(STATUS "Enabling AddressSanitizer and UndefinedBehaviorSanitizer")
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# Set up clang-tidy (automatic during build)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" ON)
find_program(CLANG_TIDY_EXE NAMES "clang-tidy-20" "clang-tidy")
if(CLANG_TIDY_EXE AND ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
endif()

# Find dependencies
find_package(GTest REQUIRED)

# Main executable
add_executable(adachess
    src/main.cpp
)

target_include_directories(adachess PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Install target
install(TARGETS adachess DESTINATION bin)

# Tests
enable_testing()

add_executable(adachess_tests
    tests/test_main.cpp
    tests/test_version.cpp
)

target_include_directories(adachess_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(adachess_tests
    GTest::gtest
    GTest::gtest_main
)

add_test(NAME unit_tests COMMAND adachess_tests)

# Custom targets for code formatting
find_program(CLANG_FORMAT_EXE NAMES "clang-format-20" "clang-format")
if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE ALL_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/include/**/*.hpp
        ${CMAKE_SOURCE_DIR}/tests/*.cpp
    )
    if(ALL_SOURCE_FILES)
        add_custom_target(format
            COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCE_FILES}
            COMMENT "Formatting source files with clang-format"
        )
        add_custom_target(format-check
            COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_SOURCE_FILES}
            COMMENT "Checking code formatting"
        )
    endif()
endif()

# Custom target for manual clang-tidy run
if(CLANG_TIDY_EXE)
    file(GLOB_RECURSE SRC_FILES
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/tests/*.cpp
    )
    if(SRC_FILES)
        add_custom_target(run-clang-tidy
            COMMAND ${CLANG_TIDY_EXE}
                -p ${CMAKE_BINARY_DIR}
                --extra-arg=-I${CMAKE_SOURCE_DIR}/include
                ${SRC_FILES}
            COMMENT "Running clang-tidy on source files"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
    endif()
endif()

# Custom target for clean
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMENT "Cleaning build directory"
)
