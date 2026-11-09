cmake_minimum_required(VERSION 3.20)

project(SDLGame
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
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
    src/TaskCard.cpp
    src/Function.cpp
    src/CharacterReaction.cpp
)

target_link_libraries(SDLGame PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    SDL3_mixer::SDL3_mixer
    SDL3_net::SDL3_net
)

include(CTest)
if(BUILD_TESTING)
    add_executable(CharacterReactionTests
        tests/CharacterReactionTest.cpp
        src/CharacterReaction.cpp
    )
    target_include_directories(CharacterReactionTests PRIVATE src)
    target_link_libraries(CharacterReactionTests PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
    )
    add_test(NAME CharacterReaction COMMAND CharacterReactionTests)
    set_tests_properties(CharacterReaction PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
