cmake_minimum_required(VERSION 2.6)

#project name
project(Arty)

#specific compiler settings
set(CMAKE_CXX_FLAGS " -g -Wall -fopenmp  -O3 -m64  -pipe -fmessage-length=0 -D_FORTIFY_SOURCE=2 -fstack-protector -funwind-tables -fasynchronous-unwind-tables -D_REENTRANT -lz -lpng -DNO_FREETYPE -Wno-write-strings")



# collect all sources (copy from 'ls -1 *.h *.cpp')
SET(ArtySources 
Color.cpp
Color.h
common.h
Image.cpp
Image.h
Texture.h
MonochromaticTexture.h
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

