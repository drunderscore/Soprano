cmake_minimum_required(VERSION 3.2)

project(Soprano)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(LibSoprano)

add_executable(Soprano
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_sdl.cpp
    GL/gl3w.c
    Main.cpp)

target_include_directories(Soprano PUBLIC SYSTEM imgui SDL/win/include LibSoprano .)
target_link_directories(Soprano PUBLIC SDL/win/lib/x64)
target_link_libraries(Soprano PUBLIC SDL2 OpenGL32 LibSoprano)
target_compile_definitions(Soprano PUBLIC SDL_MAIN_HANDLED WIN32_LEAN_AND_MEAN IMGUI_IMPL_OPENGL_LOADER_CUSTOM="GL/gl3w.h")
