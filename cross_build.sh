#!/bin/bash
# windows binary cross compiled with MXE (http://mxe.cc/)
# by thorsten kattanek 2016

if [ $# -ne 3 ] ; then       #Solange die Anzahl der Parameter ($#) größer 0
   echo ""
   echo "Fehler: Parameter fehlen ! (cross_build.sh MXE-Pfad Zielarchitektur Linktyp)"
   echo ""
   echo "Beispiel: ./cross_build.sh /media/daten/mxe i686 static"
   echo ""
   exit
fi

# Alle 3 Parameter zwischenspeichern
MXE_ROOT_PATH="${1}"
ARCH="${2}"
LINKTYP="${3}"

###############################################################################

# Pfade zusammenstellen
BUILD_PATH="./windows-${2}-${3}-build"
MXE_USR_PATH="$MXE_ROOT_PATH/usr"
MXE_BIN_PATH="$MXE_USR_PATH/bin"

# Architektur Name für mingw zusammenstellen
ARCHNAME="${ARCH}-w64-mingw32.${LINKTYP}"

# Alle Parameter ausgeben
echo
echo "+++++++++++++++++++++++++++++++++++++++++++"
echo "MXE Root Pfad = ${MXE_ROOT_PATH}"
echo "Architektur = ${ARCH}"
echo "Linktyp = ${LINKTYP}"
echo "Name = ${ARCHNAME}"
echo "+++++++++++++++++++++++++++++++++++++++++++"
echo 

# Prüfen ob für die gewählte Architektur unter MXE eine copilierte Version vorhanden ist
if [ -d "${MXE_USR_PATH}/${ARCH}-w64-mingw32.${LINKTYP}" ] ; then
echo "Pakete für ${ARCH}-w64-mingw32.${LINKTYP} gefunden."
else
echo "Keine Pakete für ${ARCH}-w64-mingw32.${LINKTYP} gefunden !"
exit
fi

export PATH="$MXE_BIN_PATH":$PATH

# check of exist build dir
if [ -d "$BUILD_PATH" ] ; then
# build dir ist exist
# remove
rm -rf "$BUILD_PATH"
# create build dir
mkdir "$BUILD_PATH"
else
# build dir is not exist
# create build dir
mkdir "$BUILD_PATH"
fi

# change to build dir
cd "$BUILD_PATH"

# cmake run
cmake -L -DCMAKE_BUILD_TYPE=Release\
      -DCMAKE_TOOLCHAIN_FILE="$MXE_USR_PATH/${ARCHNAME}/share/cmake/mxe-conf.cmake"\
      -DCMAKE_SDL_LIBS="-Wl,-subsystem,console `${ARCHNAME}-sdl-config --libs` `${ARCHNAME}-pkg-config SDL_image SDL_mixer SDL_gfx --libs`"\
      -DCMAKE_SDL2_LIBS="-Wl,-subsystem,console `${ARCHNAME}-sdl2-config --libs` `${ARCHNAME}-pkg-config SDL2_image SDL2_mixer SDL2_gfx --libs`"\
      -DCMAKE_GLEW_LIBS="`${ARCHNAME}-pkg-config glew --libs`"\
      -DCMAKE_SFML_LIBS="`${ARCHNAME}-pkg-config sfml --libs`"\
      -DCMAKE_MODPLUG_LIBS="`${ARCHNAME}-pkg-config libmodplug --libs`"\
      ..
# build run
make -j8