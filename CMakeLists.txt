cmake_minimum_required(VERSION 2.8)

set(src src/main.cpp src/komplex.cpp)
add_executable(sdl_apfel ${src})
target_link_libraries(sdl_apfel SDLmain SDL SDL_gfx)