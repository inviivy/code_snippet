cmake_minimum_required(VERSION 3.15)

project(TinyIM VERSION 0.1
                 DESCRIPTION "TinyIM"
                 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)


# 查找 protobuf
find_package(Protobuf 3 REQUIRED)
if (PROTOBUF_FOUND)
    message("protobuf found")
else ()
    message(FATAL_ERROR "Cannot find Protobuf")
endif ()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

list(APPEND CXX_FLAGS
            "-fno-rtti")

list(APPEND SANITIZE_FLAG
            "-fsanitize=address"
            "-fsanitize=leak"
            "-fsanitize=undefined")

list(APPEND CXX_FLAGS_DEBUG
            "-O0"
            "-DDBUG"
            ${SANITIZE_FLAG})
            #"-Wsuggest-final-types"
            #"-Wsuggest-final-methods"
            #"-Wsuggest-override")

list(APPEND CXX_FLAGS_RELEASE
            "-O3"
            "-Wall"
            "-Wextra"
            "-Werror"
            "-Wconversion"
            "-Wno-unused-parameter"
            "-Wno-unused")

set(CMAKE_VERBOSE_MAKEFILE ON)


# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(apps)

enable_testing()
#if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(tests)
endif()
