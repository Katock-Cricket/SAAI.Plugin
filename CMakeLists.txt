cmake_minimum_required(VERSION 3.23)

project(saai.plugin)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

set(CURL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/curl/include")
set(CURL_LIBRARY "${CMAKE_SOURCE_DIR}/thirdparty/curl/lib/libcurl.dll.a")

find_package(CURL REQUIRED)

add_definitions(-DCURL_STATICLIB)


add_subdirectory(thirdparty/plugin-sdk-build)
add_subdirectory(source)
