cmake_minimum_required(VERSION 3.23)

project(saai.plugin)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(SAAI_DIR "${CMAKE_SOURCE_DIR}/saai")
set(INCLUDES "${SAAI_DIR}/include")

# include curl
set(CURL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/curl/include")
set(CURL_LIBRARY "${CMAKE_SOURCE_DIR}/thirdparty/curl/lib/libcurl_a.lib")
find_package(CURL REQUIRED)
add_definitions(-DCURL_STATICLIB)

# build sdk
add_subdirectory(thirdparty/plugin-sdk-build)

# build saai
file(GLOB SOURCES
        ${SAAI_DIR}/src/*.cpp
)
add_library(saai SHARED
        ${SOURCES}
)

target_include_directories(saai PRIVATE
        ${CURL_INCLUDE_DIR}
        ${INCLUDES}/chatbot/
        ${INCLUDES}/config/
        ${INCLUDES}/core/
        ${INCLUDES}/shared/
        ${INCLUDES}/svc-client/
        ${INCLUDES}/unittest/
)

target_link_libraries(saai PRIVATE
        gtasa
        ${CURL_LIBRARY}
        Ws2_32
        Wldap32
        winmm
        Crypt32
        Normaliz
)
target_compile_definitions(saai PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
set_target_properties(saai PROPERTIES PREFIX "")
set_target_properties(saai PROPERTIES SUFFIX ".asi")
