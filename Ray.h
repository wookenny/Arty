#pragma once

#include "Vector3.h"

class Ray{
	
private:
	Vector3 _origin;
	Vector3 _direction;
	int _raydepth;
	
public:
	Ray(const Vector3& o=Vector3(0,0,0), const Vector3& d=Vector3(0,0,1), int r = 4):_origin(o),_direction(d),_raydepth(r){};
	Ray(const Ray& r):_origin(r._origin),_direction(r._direction),_raydepth(r._raydepth){};

	//getter
	const Vector3& getOrigin() const{ return _origin; }	
	const Vector3& getDirection() const{ return _direction; }	
	int getDepth() const{ return _raydepth; }


	//setter
	void setOrigin(const Vector3& o){ _origin = o; }
	void setDirection(const Vector3& d){ _direction = d; }
	void setRaydepth(int r){ _raydepth = r;}

	//static
	static Ray getReflectedRay(const Ray& ray, real t, const Vector3& normal){
		Vector3 newOrigin = ray.getOrigin()+t*ray.getDirection();
		Vector3 newDirection = ray.getDirection() - 2*(normal.dot(ray.getDirection()))*normal;
		return Ray(newOrigin,newDirection, ray.getDepth()-1);	
	}

};
