add_subdirectory(fmt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(LibSoprano STATIC
    ${CMAKE_SOURCE_DIR}/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ChatComponent.cpp
    Color.cpp
    )

target_include_directories(LibSoprano PUBLIC SYSTEM ${CMAKE_SOURCE_DIR} fmt/include .)
target_link_libraries(LibSoprano PUBLIC fmt)
target_compile_definitions(LibSoprano PRIVATE IMGUI_DEFINE_MATH_OPERATORS)
