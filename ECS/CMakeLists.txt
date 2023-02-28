cmake_minimum_required(VERSION 3.15)

project(miniECS VERSION 0.1
                 DESCRIPTION "miniECS"
                 LANGUAGES CXX)

include(FetchContent)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_VERBOSE_MAKEFILE off)

# release需要手动关闭
set(IS_ENABLE_SANITIZE on CACHE BOOL "" FORCE)

list(APPEND CXX_FLAGS
            "-fno-rtti")

list(APPEND SANITIZE_FLAG "")

if(IS_ENABLE_SANITIZE)
    list(APPEND SANITIZE_FLAG
            "-fno-omit-frame-pointer"
            "-fsanitize=address"
            "-fsanitize=leak"
            "-fsanitize=undefined")
endif()


list(APPEND CXX_FLAGS_DEBUG
            "-Og"
            "-DDBUG")
            #"-Wsuggest-final-types"
            #"-Wsuggest-final-methods"
            #"-Wsuggest-override")

if(IS_ENABLE_SANITIZE)
    list(APPEND CXX_FLAGS_DEBUG ${SANITIZE_FLAG})
endif()

list(APPEND CXX_FLAGS_RELEASE
            "-O3"
            "-Wall"
            "-Wextra"
            "-Werror"
            "-Wconversion"
            "-Wshadow"
            "-Wno-unused-parameter"
            "-Wno-unused")

# The compiled library code is here
add_subdirectory(src)

enable_testing()
#if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(tests)
endif()