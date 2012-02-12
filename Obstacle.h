#pragma once

#include "common.h"
#include "Vector3.h"
#include "Ray.h"
#include "Material.h"
#include <utility>
#include <cmath> 
#include <limits>

struct IntersectionCompound{
	Material *mat;
	real t;
	Vector3 normal;
	real px,py; //barycentric coords for 2d textures
	Vector3 hitpoint; //real coordinates for 3d textures	
        
	 IntersectionCompound():mat(0),t(std::numeric_limits<real>::infinity() ),normal(1,0,0),px(0),py(0){}

};

class Obstacle{ 

private:
	//Materialpointer
	Vector3 _movement;
	Material *_mat;

public: 
	virtual IntersectionCompound getIntersection(const Ray&) const = 0; //get Color, Normal and Material of Intersection
	//get and set Material	
	Material* getMaterial() const{ return _mat;}
	void setMaterial(Material* m){ _mat = m; }
	  
}; 
