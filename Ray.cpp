#include "Ray.h"

real Ray::intersectBox(const Vector3 &boxMin, const Vector3 &boxMax, const Vector3 &pos, bool debug) const{
	real tmin, tmax, tymin, tymax, tzmin, tzmax;
	Vector3 bounds[2] = {boxMin,boxMax}; 
	tmin =  (bounds[_sign[0]]   [0]  -  pos[0])/_direction[0];// * _inv_direction[0];
	tmax =  (bounds[1-_sign[0]] [0]  -  pos[0])/_direction[0];// * _inv_direction[0];
	tymin = (bounds[_sign[1]]   [1]  -  pos[1])/_direction[1];// * _inv_direction[1];
	tymax = (bounds[1-_sign[1]] [1]  -  pos[1])/_direction[1];// * _inv_direction[1];
	if(debug){
		std::cout<<"x variable  min: "<<tmin<<" max: "<<tmax<<std::endl;
		std::cout<<"y variable  min: "<<tymin<<" max: "<<tymax<<std::endl;		
	}	
	
	if ( (tmin > tymax) or (tymin > tmax) ){
		return -1;
	}
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;


	tzmin = (bounds[_sign[2]]  [2]   - pos[2])/_direction[2];// * _inv_direction[2];
	tzmax = (bounds[1-_sign[2]][2]   - pos[2])/_direction[2];// * _inv_direction[2];
	if(debug){
		std::cout<<"z variable  min: "<<tzmin<<" max: "<<tzmax<<std::endl;
	}	
	
	
	if ( (tmin > tzmax) or (tzmin > tmax) )
		return -1;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	if(debug){
		std::cout<<"solution=  min: "<<tmin<<" max: "<<tmax<<std::endl;
	}	
	//hits could be e.g. -1,1
	if (tmin>0){
		if(debug)	std::cout<<"returning "<<tmin<<std::endl;
		return tmin;
	}
	if(debug)	std::cout<<"returning "<<tmax<<std::endl;
	return tmax;
}
