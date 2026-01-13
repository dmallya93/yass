cmake_minimum_required(VERSION 3.20)
project(yass VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer Clang for development
if(NOT CMAKE_CXX_COMPILER_ID)
    find_program(CLANG_CXX clang++)
    if(CLANG_CXX)
        set(CMAKE_CXX_COMPILER ${CLANG_CXX})
    endif()
endif()

# Enable modern C++ warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
elseif(MSVC)
    add_compile_options(/W4 /permissive-)
endif()

# Set up clang-tidy (automatic during build)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" ON)
find_program(CLANG_TIDY_EXE NAMES "clang-tidy-20" "clang-tidy-14" "clang-tidy")
if(CLANG_TIDY_EXE AND ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()

# Find cmark library
find_library(CMARK_LIBRARY NAMES cmark libcmark REQUIRED)
find_path(CMARK_INCLUDE_DIR NAMES cmark.h REQUIRED)

# Config library
add_library(config
    src/config/config.cpp
)

target_include_directories(config
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Messages library
add_library(messages
    src/messages/messages.cpp
)

target_include_directories(messages
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Markdown wrapper library
add_library(markdown
    src/markdown/cmark.cpp
)

target_include_directories(markdown
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE
        ${CMARK_INCLUDE_DIR}
)

target_link_libraries(markdown
    PRIVATE
        ${CMARK_LIBRARY}
)

# Sanitizer support: Enabled by default in Debug builds
option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" ON)
if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        message(STATUS "Enabling AddressSanitizer and UndefinedBehaviorSanitizer")
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# Enable testing
enable_testing()

# Find Google Test
find_package(GTest QUIET)

# Test executables
add_executable(test_cmark
    tests/test_cmark.cpp
)

target_link_libraries(test_cmark
    PRIVATE
        markdown
)

target_include_directories(test_cmark
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_test(NAME test_cmark COMMAND test_cmark)

# Config tests (only if GTest is available)
if(GTest_FOUND)
    add_executable(test_config
        tests/test_config.cpp
    )

    target_link_libraries(test_config
        PRIVATE
            config
            GTest::gtest
            GTest::gtest_main
    )

    target_include_directories(test_config
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    add_test(NAME test_config COMMAND test_config)

    # Messages tests
    add_executable(test_messages
        tests/test_messages.cpp
    )

    target_link_libraries(test_messages
        PRIVATE
            messages
            GTest::gtest
            GTest::gtest_main
    )

    target_include_directories(test_messages
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    add_test(NAME test_messages COMMAND test_messages)
else()
    message(STATUS "Google Test not found, skipping config and messages tests")
endif()
