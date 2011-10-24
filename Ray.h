#pragma once

#include "Vector3.h"

class Ray{
	
private:
	Vector3 _origin;
	Vector3 _direction;
	int _raydepth;
	//used multiple times, so it is stored
	Vector3 _inv_direction;
	int _sign[3];
	
	void _initAdditionalData(){
		_inv_direction = Vector3(1/_direction[0], 1/_direction[1], 1/_direction[2]);
		_sign[0] = (_inv_direction[0] < 0);
		_sign[1] = (_inv_direction[1] < 0);
		_sign[2] = (_inv_direction[1] < 0);
	}

public:
	Ray(const Vector3& o=Vector3(0,0,0), const Vector3& d=Vector3(0,0,1), int r = 4):_origin(o),_direction(d),_raydepth(r){
		_initAdditionalData();
	}
	Ray(const Ray& r):_origin(r._origin),_direction(r._direction),_raydepth(r._raydepth){_initAdditionalData();}

	//getter
	const Vector3& getOrigin() const{ return _origin; }	
	const Vector3& getDirection() const{ return _direction; }	
	Vector3& Direction() { return _direction; }		
	int getDepth() const{ return _raydepth; }


	//setter
	void setOrigin(const Vector3& o){ _origin = o; }
	void setDirection(const Vector3& d){ _direction = d; _initAdditionalData();}
	void setRaydepth(int r){ _raydepth = r;}


	real intersectBox(const Vector3 &boxMin, const Vector3 &boxMax) const;



	//static
	static Ray getReflectedRay(const Ray& ray, real t, const Vector3& normal){
		Vector3 newOrigin = ray.getOrigin()+t*ray.getDirection();
		Vector3 newDirection = ray.getDirection() - 2*(normal.dot(ray.getDirection()))*normal;
		return Ray(newOrigin,newDirection, ray.getDepth()-1);	
	}

};
