cmake_minimum_required(VERSION 3.24)
project(C C)

set(CURL_INCLUDE_DIR "E:/Curl/include")
set(CURL_LIBRARY "E:/Curl/lib/libcurl.dll.a")  # For MinGW
# set(CURL_LIBRARY "E:/Curl/lib/libcurl.lib")  # For Visual Studio
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)
include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})
include_directories(${CURL_INCLUDE_DIR})
link_directories("E:/Curl/lib")

set(CMAKE_C_STANDARD 11)

add_executable(my_executable tasty.c cJSON.c)

target_link_libraries(my_executable ${CURL_LIBRARY})