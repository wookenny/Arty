#include "Sphere.h"
#include <cmath>

IntersectionCompound Sphere::getIntersection(const Ray& ray) const{
	IntersectionCompound c;
	c.mat = getMaterial();
	c.t = -1;
	c.px=c.py=-1;
	//mehod from http://www.devmaster.net/wiki/Ray-sphere_intersection
	Vector3 dst = ray.getOrigin() - _center;
	real B = dst.dot(ray.getDirection());//assume direction is a unit vector
	real C = dst.dot(dst) - _radius2;
	real D = B*B - C;
	
	if( D > 0){ //ray hits sphere?
		real param = -B - sqrt(D);
		c.t = param;
		Vector3 &hitpoint = c.hitpoint;
		hitpoint = ray.getOrigin() + param*ray.getDirection();
		c.normal = (hitpoint -_center).normalize();
		//parametric coords TODO: find formula
		c.px = (_radius + hitpoint[0] - _center[0])/(2*_radius);
		c.py = (_radius + hitpoint[1] - _center[1])/(2*_radius);
	}

	return c;
}
