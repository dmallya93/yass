cmake_minimum_required(VERSION 3.20)
project(YASS VERSION 1.0.0 LANGUAGES CXX)

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
    message(STATUS "clang-tidy enabled: ${CLANG_TIDY_EXE}")
endif()

# Conan dependencies - find packages
find_package(spdlog REQUIRED)
find_package(GTest REQUIRED)

# Logging library
add_library(yass_logging
    src/yass/logging/logging.cpp
)

target_include_directories(yass_logging
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(yass_logging
    PUBLIC
        spdlog::spdlog
)

# Config library
add_library(yass_config
    src/yass/config/config.cpp
)

target_include_directories(yass_config
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(yass_config
    PUBLIC
        yass_logging
)

# Sanitizer support: Enabled by default in Debug builds
option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" ON)
if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        message(STATUS "Enabling AddressSanitizer and UndefinedBehaviorSanitizer")
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# Tests
enable_testing()

add_executable(test_logging
    tests/unit/test_logging.cpp
)

target_link_libraries(test_logging
    PRIVATE
        yass_logging
        GTest::gtest_main
)

add_test(NAME LoggingTests COMMAND test_logging)

add_executable(test_config
    tests/unit/test_config.cpp
)

target_link_libraries(test_config
    PRIVATE
        yass_config
        GTest::gtest_main
)

add_test(NAME ConfigTests COMMAND test_config)

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
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
        add_custom_target(format-check
            COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_SOURCE_FILES}
            COMMENT "Checking code formatting"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
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
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
    endif()
endif()
