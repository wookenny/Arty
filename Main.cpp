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

