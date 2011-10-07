#pragma once

#include "common.h"


/********************************************************/
/* header for AABB-triangle overlap test code                      */
/* by Tomas Akenine-Möller                              */
/* Function: int triBoxOverlap(float boxcenter[3],      */
/*          float boxhalfsize[3],float triverts[3][3]); */
/********************************************************/

bool triBoxOverlap(real boxcenter[3],real boxhalfsize[3], real tri[3][3 ]);

