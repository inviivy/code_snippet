add_executable(main main.cpp World.hpp)

target_compile_features(main PUBLIC cxx_std_20)

target_compile_options(main PRIVATE
            ${CXX_FLAGS}
            "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>"
            "$<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>")

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND IS_ENABLE_SANITIZE)
    message("-------------------------")
    target_link_options(main PRIVATE ${SANITIZE_FLAG})
endif()