cmake_minimum_required(VERSION 3.15)

project(MyRaylibGame)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(raylib)

add_executable(game 
    main.cpp
    camera_cpp/camera.cpp
    usefullFunc_cpp/usefullFunc.cpp
    chunk_cpp/chunk.cpp
    rendering/redering.cpp
    movement_cpp/movement.cpp
    Entity_cpp/Entity.cpp
    movement_cpp/collisionCheck.cpp
    mouse_cpp/mouseDirectionSeeing.cpp
    rendering/GUI_cpp/hotBar.cpp
    rendering/GUI_cpp/GUI.cpp
    blocks.cpp
    "FastNoiseLite.h"
    "main.h" 
    "particles_cpp/particles.cpp" 
    "particles_cpp/particles.h")

target_include_directories(game PRIVATE
    ${CMAKE_SOURCE_DIR}/usefullFunc_cpp
)

file(COPY ${CMAKE_SOURCE_DIR}/textures DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(game PRIVATE raylib)

if (PLATFORM STREQUAL "Web")

    set_target_properties(game PROPERTIES SUFFIX ".html")

    target_link_options(game PRIVATE
        "-sUSE_GLFW=3"
        "-sASYNCIFY"
        "-sALLOW_MEMORY_GROWTH"
        "--preload-file"
        "textures"
    )

endif()

add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:game>/shaders
    $<TARGET_FILE_DIR:game>/shaders
)