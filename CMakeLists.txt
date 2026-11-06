cmake_minimum_required(VERSION 3.20)

project(SDLGame
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## ライブラリの探索
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(SDL3_mixer CONFIG REQUIRED)
find_package(SDL3_net CONFIG REQUIRED)


add_executable(SDLGame ## WIN32はRelease版用,GUIアプリとしてビルド
    src/main.cpp
    src/SDLfps.cpp
    src/SDLTimer.cpp
    src/Window.cpp
)

target_link_libraries(SDLGame PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    SDL3_mixer::SDL3_mixer
    SDL3_net::SDL3_net
)
