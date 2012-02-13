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

	//DEBUG KD-Tree
	/*
	KDTree kd_tree;

	kd_tree.traverse();

	Vector3 a1(0,1,2), b1(2,3,5), c1(2,3,1);
	Vector3 a2(0,0.1,0.2), b2(0.1,0,0.4), c2(0,0.5,0.5);
	Vector3 a3(6,6.1,6.2), b3(6.1,6,6.4), c3(6,6.5,6.5);
	Triangle t1(a1,b1,c1);	
	Triangle t2(a2,b2,c2);
	Triangle t3(a3,b3,c3);

	kd_tree.addTriangle(t1);
	kd_tree.addTriangle(t2);
	kd_tree.addTriangle(t3);	
	kd_tree.init();
	kd_tree.traverse();
	*/
	
	 /*
	//Intersection test(Debugging)
	//triangle: ( -1, -1, 1 ), ( -1, 1, 3 ), ( -1, -1, 3 )
	//box left: [-1;-0.3]x[-1;1]x[1;3]
	//box right: [-0.3;1]x[-1;1]x[1;3]
	Triangle t(Vector3(-1,-1,1),Vector3(-1,1,3),Vector3(-1,-1,3));
	Vector3 boxLeftMin(-1,-1,1);
	Vector3 boxLeftMax(-0.3,1,3);
	Vector3 boxRightMin(-0.3,-1,1);
	Vector3 boxRightMax(1,1,3);
	std::cout<<std::boolalpha;
	
	std::cout<< "t intersects leftBox: "<< t.overlapAABB(boxLeftMin,boxLeftMax) << std::endl; 
	std::cout<< "t intersects rightBox: "<< t.overlapAABB(boxRightMin,boxRightMax) << std::endl; 
	
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

