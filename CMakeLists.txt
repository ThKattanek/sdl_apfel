cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s -Os")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s -Os")

set(SOURCE src/main.cpp src/komplex.cpp)
add_executable(sdl_apfel ${SOURCE})

if(UNIX)
target_link_libraries(sdl_apfel SDL SDLmain SDL_gfx)
endif(UNIX)

if(WIN32)
target_link_libraries(sdl_apfel ${CMAKE_SDL_LIBS})
endif(WIN32)