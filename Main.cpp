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
	if(args==2){
		rt.loadScene(std::string(argv[1]));
	}else
		rt.loadDefaultScene();
	rt.trace(); 
	rt.saveImage("traced.png");
}

