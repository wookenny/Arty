cmake_minimum_required(VERSION 2.6)

#project name
project(Arty)

#specific compiler settings
set(CMAKE_CXX_FLAGS "-g -Wall -fopenmp -DN_DEBUG -mfpmath=sse -O3 -m64 -pipe -fmessage-length=0 -D_FORTIFY_SOURCE=2 -fstack-protector -funwind-tables -fasynchronous-unwind-tables -D_REENTRANT -lz -lpng -DNO_FREETYPE -Wno-write-strings -std=c++0x -I.")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS " -lz -lpng -pthread -L.")

# collect all sources (copy from 'ls -1 *.cpp')
SET(ArtySources
AABB-triangle-overlap.cpp
Color.cpp
Color.h
common.h
common.cpp
Image.cpp
Image.h
Texture.h
MonochromaticTexture.h
ChessboardTexture.h
ImageTexture.h
KDTree.h
KDTree.cpp
Lightsource.h
Lightsource.cpp
Main.cpp
Material.h
Obstacle.h
PerlinNoise.cpp
PerlinNoise.h
PerlinTexture.h
pngwriter.cpp
pngwriter.h
pugiconfig.hpp
pugixml.hpp
pugixml.cpp
Ray.h
Ray.cpp
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

# collect all sources
SET(GTestSources
MathFunction_unittest.cpp
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
ADD_EXECUTABLE(GTests ${GTestSources} )
target_link_libraries(GTests -lgtest -lgtest_main)
