add_executable(timer1 timer1.cpp)
target_compile_features(timer1 PRIVATE cxx_std_20)

target_link_libraries(timer1 PRIVATE ${Boost_LIBRARIES} pthread)


target_compile_options(timer1 PRIVATE
            ${CXX_FLAGS}
            "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>"
            "$<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>")

#target_compile_options(app PRIVATE "-fsanitize=address" "-fsanitize=undefined")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_options(timer1 PRIVATE ${SANITIZE_FLAG})
endif()



add_executable(timer2 timer2.cpp)
target_compile_features(timer2 PRIVATE cxx_std_20)

target_link_libraries(timer2 PRIVATE ${Boost_LIBRARIES} pthread)


target_compile_options(timer2 PRIVATE
            ${CXX_FLAGS}
            "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>"
            "$<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>")

#target_compile_options(app PRIVATE "-fsanitize=address" "-fsanitize=undefined")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_options(timer2 PRIVATE ${SANITIZE_FLAG})
endif()


add_executable(timer3 timer3.cpp)
target_compile_features(timer3 PRIVATE cxx_std_20)

target_link_libraries(timer3 PRIVATE ${Boost_LIBRARIES} pthread)


target_compile_options(timer3 PRIVATE
            ${CXX_FLAGS}
            "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>"
            "$<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>")

#target_compile_options(app PRIVATE "-fsanitize=address" "-fsanitize=undefined")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_options(timer3 PRIVATE ${SANITIZE_FLAG})
endif()



add_executable(timer4 timer4.cpp)
target_compile_features(timer4 PRIVATE cxx_std_20)

target_link_libraries(timer4 PRIVATE ${Boost_LIBRARIES} pthread)


target_compile_options(timer4 PRIVATE
            ${CXX_FLAGS}
            "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>"
            "$<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>")

#target_compile_options(app PRIVATE "-fsanitize=address" "-fsanitize=undefined")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_options(timer4 PRIVATE ${SANITIZE_FLAG})
endif()




add_executable(timer5 timer5.cpp)
target_compile_features(timer5 PRIVATE cxx_std_20)

target_link_libraries(timer5 PRIVATE ${Boost_LIBRARIES} boost_thread pthread)


target_compile_options(timer5 PRIVATE
            ${CXX_FLAGS}
            "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>"
            "$<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>")

#target_compile_options(app PRIVATE "-fsanitize=address" "-fsanitize=undefined")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_options(timer5 PRIVATE ${SANITIZE_FLAG})
endif()

add_subdirectory(socket)
