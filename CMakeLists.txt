cmake_minimum_required(VERSION 3.20)
project(yass VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Prefer Clang for development
if(NOT CMAKE_CXX_COMPILER_ID)
    find_program(CLANG_CXX clang++)
    if(CLANG_CXX)
        set(CMAKE_CXX_COMPILER ${CLANG_CXX})
    endif()
endif()

# Enable modern C++ warnings (Clang-optimized)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
elseif(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Set up clang-tidy (automatic during build)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" ON)
find_program(CLANG_TIDY_EXE NAMES "clang-tidy-20" "clang-tidy")
if(CLANG_TIDY_EXE AND ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()

# Find dependencies
find_package(GTest REQUIRED)
# For Ubuntu 22.04, we need to link against pthread
find_package(Threads REQUIRED)
# Find cmark library for markdown processing
find_package(PkgConfig REQUIRED)
pkg_check_modules(CMARK REQUIRED libcmark)

# Messages library
add_library(messages
    src/messages/messages.cpp
)

target_include_directories(messages PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Config library
add_library(config
    src/config/config.cpp
)

target_include_directories(config PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Layouts library
add_library(layouts
    src/layouts/layouts.cpp
)

target_include_directories(layouts PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(layouts PUBLIC
    messages
)

# CMark wrapper library
add_library(cmark_wrapper
    src/cmark/cmark_wrapper.cpp
)

target_include_directories(cmark_wrapper PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMARK_INCLUDE_DIRS}
)

target_link_libraries(cmark_wrapper PUBLIC
    ${CMARK_LIBRARIES}
)

# Pages library
add_library(pages
    src/pages/pages.cpp
)

target_include_directories(pages PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pages PUBLIC
    config
    cmark_wrapper
    messages
)

target_link_libraries(config PUBLIC
    messages
)

# Main executable
add_executable(yass
    src/main.cpp
)

target_include_directories(yass PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(yass PRIVATE
    config
    cmark_wrapper
    layouts
    messages
    pages
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

# Tests
enable_testing()

add_executable(layouts_test
    tests/layouts_test.cpp
)

target_link_libraries(layouts_test
    layouts
    GTest::gtest_main
    Threads::Threads
)

target_include_directories(layouts_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_test(NAME layouts_test COMMAND layouts_test)

# Pages unit test
add_executable(pages_test
    tests/pages_test.cpp
)

target_link_libraries(pages_test
    pages
    config
    GTest::gtest_main
    Threads::Threads
)

target_include_directories(pages_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_test(NAME pages_test COMMAND pages_test)

# Integration test
add_executable(integration_test
    tests/integration_test.cpp
)

target_link_libraries(integration_test
    GTest::gtest_main
    Threads::Threads
)

target_include_directories(integration_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_test(NAME integration_test COMMAND integration_test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Custom targets for code formatting
find_program(CLANG_FORMAT_EXE NAMES "clang-format-20" "clang-format")
if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE ALL_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/include/*.hpp
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
    file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
    if(SRC_FILES)
        add_custom_target(run-clang-tidy
            COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${SRC_FILES}
            COMMENT "Running clang-tidy on source files"
        )
    endif()
endif()
