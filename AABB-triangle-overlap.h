#pragma once

#include "common.h"

//needed forward declarations
class Triangle;
class Vector3;

/********************************************************/
/* header for AABB-triangle overlap test code                      */
/* by Tomas Akenine-Möller                              */
/* Function: int triBoxOverlap(float boxcenter[3],      */
/*          float boxhalfsize[3],float triverts[3][3]); */
/********************************************************/


bool triBoxOverlap(const Vector3 &boxMin,const Vector3 &boxMax, const Triangle *tri);
inline bool triBoxOverlap(const Vector3 &boxMin,const Vector3 &boxMax, const Triangle &tri){
			return triBoxOverlap(boxMin, boxMax, &tri);}

