cmake_minimum_required(VERSION 2.6)

#project name
project(Arty)

#specific compiler settings
#deleted flags(pathCC does not know them):  -mfpmath=sse -fstack-protector
#-DN_DEBUG -O3 -m64 -pipe -fmessage-length=0 -D_FORTIFY_SOURCE=2  -funwind-tables -fasynchronous-unwind-tables -D_REENTRANT -lz -lpng -DNO_FREETYPE -Wno-write-strings -Woverloaded-virtual
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++0x -DNO_FREETYPE -lz -lpng -DN_DEBUG -O3 -m64 -pipe -fmessage-length=0 -D_FORTIFY_SOURCE=2  -funwind-tables -fasynchronous-unwind-tables -pthread -D_REENTRANT -lz -lpng -DNO_FREETYPE -Wno-write-strings -Woverloaded-virtual")



# collect all sources (copy from 'ls -1 *.h *.cpp')
SET(ArtySources
Color.cpp
Color.h
common.h
Image.cpp
Image.h
Texture.h
MonochromaticTexture.h
ChessboardTexture.h
ImageTexture.h
Lightsource.h
Main.cpp
Material.h
Obstacle.h
pngwriter.cpp
pngwriter.h
pugiconfig.hpp
pugixml.hpp
pugixml.cpp
Ray.h
Raytracer.cpp
Raytracer.h
Scene.h
Scene.cpp
Sphere.cpp
Sphere.h
Triangle.cpp
Triangle.h
Vector3.cpp
Vector3.h
)

# display status message for important variables
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "building with flags: " )
MESSAGE( STATUS "${CMAKE_CXX_FLAGS}" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )

#define executable
ADD_EXECUTABLE(Arty ${ArtySources} )

