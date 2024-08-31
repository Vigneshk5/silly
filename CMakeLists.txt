cmake_minimum_required(VERSION 3.10)

project(SillyBrowser)

set(CMAKE_CXX_STANDARD 17)


include_directories(include)


set(SOURCES
    src/main.cpp
    src/HTMLRenderer.cpp
)


find_package(CURL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED sdl2_ttf)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS} ${SDL2_TTF_LIBRARY_DIRS})

add_executable(SillyBrowser ${SOURCES})

target_link_libraries(SillyBrowser ${CURL_LIBRARIES} SDL2::SDL2 SDL2_ttf::SDL2_ttf)
