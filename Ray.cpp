#include "Ray.h"

real Ray::intersectBox(const Vector3 &boxMin, const Vector3 &boxMax) const {
	real tmin, tmax, tymin, tymax, tzmin, tzmax;
	Vector3 bounds[2] = {boxMin,boxMax}; 
	tmin =  (bounds[_sign[0]]   [0]  -  _origin[0]) * _inv_direction[0];
	tmax =  (bounds[1-_sign[0]] [0]  -  _origin[0]) * _inv_direction[0];
	tymin = (bounds[_sign[1]]   [1]  -  _origin[1]) * _inv_direction[1];
	tymax = (bounds[1-_sign[1]] [1]  -  _origin[1]) * _inv_direction[1];
	if ( (tmin > tymax) or (tymin > tmax) )
		return -1;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
		tzmin = (bounds[_sign[2]][2]   - _origin[2]) * _inv_direction[2];
		tzmax = (bounds[1-_sign[2]][2] - _origin[2]) * _inv_direction[2];
	if ( (tmin > tzmax) or (tzmin > tmax) )
		return -1;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	//hits could be e.g. -1,1
	if (tmin>=0)
		return tmin;
	return tmax;
}

