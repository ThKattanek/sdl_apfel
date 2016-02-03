# sdl_apfel
Apfelmänchen mit SDL

Unter Linux Kompilieren
-------------------------

cd sdl_apfel/build-linux

cmake -D CMAKE_BUILD_TYPE=Release ..
make

Unter Windows Kompilieren
-------------------------

cd sdl_apfel/build

cmake -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release ..
make (mingw32-make)