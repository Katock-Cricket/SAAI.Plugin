cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(SDK ${CMAKE_SOURCE_DIR}/thirdparty/plugin-sdk)

file(GLOB HEADERS
        ${CMAKE_SOURCE_DIR}/thirdparty/custom/*.h
        ${SDK}/plugin_sa/*.h
        ${SDK}/plugin_sa/game_sa/*.h
        ${SDK}/plugin_sa/game_sa/*/*.h
        ${SDK}/shared/*.h
        ${SDK}/shared/game/*.h
        ${SDK}/shared/extensions/*.h
        ${SDK}/hooking/*.h
        ${SDK}/injector/*.hpp
)

file(GLOB SOURCES
        ${CMAKE_SOURCE_DIR}/thirdparty/custom/*.cpp
        ${SDK}/plugin_sa/game_sa/*.cpp
        ${SDK}/shared/*.cpp
        ${SDK}/shared/game/*.cpp
        ${SDK}/shared/extensions/*.cpp
        ${SDK}/hooking/*.cpp
)
list(REMOVE_ITEM SOURCES ${SDK}/shared/Audio.cpp)

add_library(gtasa STATIC ${SOURCES} ${HEADERS})


target_compile_definitions(gtasa PUBLIC -DGTASA -DRW -D_HAS_CXX17)
target_compile_features(gtasa PRIVATE cxx_std_23)
target_include_directories(gtasa PUBLIC
        ${CMAKE_SOURCE_DIR}/thirdparty/custom/
        ${SDK}/shared/
        ${SDK}/shared/game/
        ${SDK}/shared/extensions/
        ${SDK}/plugin_sa/game_sa/
)
target_link_directories(gtasa PUBLIC
        ${SDK}/shared/bass/
)
target_sources(gtasa PRIVATE
        ${CMAKE_SOURCE_DIR}/thirdparty/custom/
        ${SDK}/plugin_sa/game_sa/
        ${SDK}/shared/game/
        ${SDK}/shared/extensions/
)
target_link_libraries(gtasa bass)