add_executable(app app.cpp)
target_compile_features(app PRIVATE cxx_std_20)
target_compile_options(app PRIVATE
            ${CXX_FLAGS}
            "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>"
            "$<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_options(app PRIVATE ${SANITIZE_FLAG})
endif()

target_link_libraries(app PRIVATE leveldb)