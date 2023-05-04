cmake_minimum_required(VERSION 3.24)
project(C C)

set(CURL_INCLUDE_DIR "E:/Curl/include")
set(CURL_LIBRARY "E:/Curl/lib/libcurl.dll.a")  # For MinGW


# gtk
include_directories(${CURL_INCLUDE_DIR})
link_directories("E:/Curl/lib")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_C_STANDARD 11)
add_executable(my_executable tasty.c cJSON.c tasty.c)

target_link_libraries(my_executable ws2_32)

target_link_libraries(my_executable ${CURL_LIBRARY})

