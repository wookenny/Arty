#include <iostream>

#include "Image.h"
#include "Color.h"
#include "common.h"
#include "Vector3.h"
#include "Sphere.h"
#include "Ray.h"
#include "Raytracer.h"
#include <string>

int main(int args, char** argv){

	/*
	//( 0.0001, 0.0001, -1.9999 )  direction: ( 0.197858, 6.94377e-05, 0.980231 )
	Vector3 pos = Vector3(0.0001, 0.0001, -1.9999 );
	Vector3 dir = Vector3( 0.300784, 0.30182, 0.904673 );
	Vector3 boxMin(-1,-1,1);
	Vector3 boxMax(1,1,3);
	Ray r(pos,dir);
	real distToBox = r.intersectBox(boxMin, boxMax, pos, true);
	std::cout<< distToBox <<std::endl;

	return 0;
	*/
	Raytracer rt;
	if(args>1){
		rt.loadScene( argv[1] );
	}else{
		rt.loadDefaultScene();
	}
	if(args>2)
		rt.trace(atoi(argv[2]));
	else
		rt.trace();
	rt.saveImage("traced.png");

}

