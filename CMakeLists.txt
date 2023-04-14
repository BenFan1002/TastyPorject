cmake_minimum_required(VERSION 3.24)
project(C C)

set(CURL_INCLUDE_DIR "E:/Curl/include")
set(CURL_LIBRARY "E:/Curl/lib/libcurl.dll.a")  # For MinGW
# set(CURL_LIBRARY "E:/Curl/lib/libcurl.lib")  # For Visual Studio

include_directories(${CURL_INCLUDE_DIR})
link_directories("E:/Curl/lib")

set(CMAKE_C_STANDARD 11)

add_executable(my_executable tasty.c)

target_link_libraries(my_executable ${CURL_LIBRARY})