cmake_minimum_required(VERSION 3.23)

project(saai)

add_library(saai SHARED
        AIMain.cpp
        AIPed.cpp
        AIPed.cpp
        ChatController.cpp
        Config.cpp
        KeyActivate.cpp
        Log.cpp
        Main.cpp
        PedSpawner.cpp
        SharedMem.cpp
        Speak.cpp
        Subtitle.cpp
        SVCClient.cpp
        ThreadPool.cpp
)

include_directories(${CURL_INCLUDE_DIR})

target_link_libraries(saai PRIVATE
        plugin_sa_lib
        ${CURL_LIBRARY}
        Ws2_32
        Wldap32
        winmm
        Crypt32
        Normaliz
)

set_target_properties(saai PROPERTIES SUFFIX ".asi")
