add_executable(syn_tcp_client syn_tcp_client.cpp)
target_compile_features(syn_tcp_client PRIVATE cxx_std_20)

target_link_libraries(syn_tcp_client PRIVATE ${Boost_LIBRARIES} boost_thread pthread)


target_compile_options(syn_tcp_client PRIVATE
            ${CXX_FLAGS}
            "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>"
            "$<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>")

#target_compile_options(app PRIVATE "-fsanitize=address" "-fsanitize=undefined")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_options(syn_tcp_client PRIVATE ${SANITIZE_FLAG})
endif()



add_executable(daytime_server daytime_server.cpp)
target_compile_features(daytime_server PRIVATE cxx_std_20)

target_link_libraries(daytime_server PRIVATE ${Boost_LIBRARIES} boost_thread pthread)


target_compile_options(daytime_server PRIVATE
            ${CXX_FLAGS}
            "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>"
            "$<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>")

#target_compile_options(app PRIVATE "-fsanitize=address" "-fsanitize=undefined")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_options(daytime_server PRIVATE ${SANITIZE_FLAG})
endif()




add_executable(async_daytime_server async_daytime_server.cpp)
target_compile_features(async_daytime_server PRIVATE cxx_std_20)

target_link_libraries(async_daytime_server PRIVATE ${Boost_LIBRARIES} boost_thread pthread)


target_compile_options(async_daytime_server PRIVATE
            ${CXX_FLAGS}
            "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>"
            "$<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>")

#target_compile_options(app PRIVATE "-fsanitize=address" "-fsanitize=undefined")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_options(async_daytime_server PRIVATE ${SANITIZE_FLAG})
endif()