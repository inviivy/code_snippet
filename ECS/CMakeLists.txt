cmake_minimum_required(VERSION 3.15)

project(ECS VERSION 0.1
                 DESCRIPTION "ECS"
                 LANGUAGES CXX)

add_executable(main main.cpp)

target_compile_features(main PRIVATE cxx_std_20)
