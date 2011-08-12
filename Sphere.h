#pragma once
#include "Obstacle.h"

class Sphere: public Obstacle{

	private:
		Vector3 _center;
		real _radius;
		real _radius2;
	public:
		IntersectionCompound getIntersection(const Ray&) const;

		//c_strs
		Sphere( const Vector3& c = Vector3(0,0,0), real r = 1.0):_center(c),_radius(r),_radius2(r*r){}

};

