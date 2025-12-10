cmake_minimum_required(VERSION 3.20)
project(yass VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable modern C++ warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wconversion -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wconversion -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
    )
elseif(MSVC)
    add_compile_options(/W4 /permissive-)
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

# Clang-tidy support - will be applied only to our targets
option(ENABLE_CLANG_TIDY "Enable clang-tidy checks" ON)
set(CLANG_TIDY_COMMAND "")
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy-20" "clang-tidy")
    if(CLANG_TIDY_EXE)
        message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
        set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy not found")
    endif()
endif()

# Google Test dependency
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Messages library
add_library(messages
    src/messages.cpp
)

target_include_directories(messages PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Apply clang-tidy only to our targets
if(CLANG_TIDY_COMMAND)
    set_target_properties(messages PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()

# Config library
add_library(config
    src/config.cpp
)

target_include_directories(config PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(config PUBLIC messages)

# Apply clang-tidy to config library
if(CLANG_TIDY_COMMAND)
    set_target_properties(config PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()

# Tests
enable_testing()

add_executable(messages_test
    tests/messages_test.cpp
)

target_link_libraries(messages_test
    messages
    GTest::gtest_main
)

# Apply clang-tidy to test targets
if(CLANG_TIDY_COMMAND)
    set_target_properties(messages_test PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()

include(GoogleTest)
gtest_discover_tests(messages_test)

add_executable(config_test
    tests/config_test.cpp
)

target_link_libraries(config_test
    config
    GTest::gtest_main
)

# Apply clang-tidy to test targets
if(CLANG_TIDY_COMMAND)
    set_target_properties(config_test PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()

gtest_discover_tests(config_test)

# Custom target for running clang-tidy manually
if(CLANG_TIDY_EXE)
    add_custom_target(run-clang-tidy
        COMMAND ${CLANG_TIDY_EXE}
        -p ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
        COMMENT "Running clang-tidy"
    )
endif()

# Installation rules (optional, for future use)
install(TARGETS messages config
    EXPORT yass-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
)
