#include "Triangle.h"
#include <sstream>
#include <cstdlib>

Triangle::Triangle( const Vector3& v1, const Vector3& v2, const Vector3& v3){
	_v1=v1; _v2=v2; _v3=v3;
	_normal = (_v2-_v1).cross(_v3-_v1);
	_normal.normalize();

	//set normals for points to 0
	_normals[0] = _normals[1] = _normals[2] = Vector3(0,0,0);
	_normalsSet = false;

	_dominant = getDominantAxis();
	
	real _b[2],_c[2];
	_b[0]= _dominant==0?(_v2[1]-_v1[1]):(_v2[0]-_v1[0]);
	_b[1]= _dominant==2?(_v2[1]-_v1[1]):(_v2[2]-_v1[2]);
	_c[0]= _dominant==0?(_v3[1]-_v1[1]):(_v3[0]-_v1[0]);
	_c[1]= _dominant==2?(_v3[1]-_v1[1]):(_v3[2]-_v1[2]);
	
	real _div1 = (_b[1]*_c[0]-_b[0]*_c[1]);
	real _div2 = (_c[1]*_b[0]-_c[0]*_b[1]);

	_ud1 =   _c[0]/_div1;
	_ud2 = - _c[1]/_div1;
	_vd1 =   _b[0]/_div2;
	_vd2 = - _b[1]/_div2;
	_px[0] = _px[1] = _px[2] = _py[0] = _py[1] = _py[2] = -1; 	
}

IntersectionCompound Triangle::getIntersection(const Ray& ray) const{
	IntersectionCompound ic;
	if( ray.getDirection().dot(_normal)>-eps )//no lightnin
		return ic;
	
	ic.mat = 0;
	ic.px=ic.py=-1;
	ic.t = -1;
	//mehod from http://www.devmaster.net/wiki/Ray-triangle_intersection

	real distance = -((ray.getOrigin()-_v1).dot(_normal))/(ray.getDirection().dot(_normal));	
	if(distance<eps){//no hit if the triangle is hit from behind
		//std::cout<<"behind object"<<std::endl;
		ic.t = -1;	
		return ic;
	}
	
	Vector3 &point = ic.hitpoint;
	point = ray.getOrigin()+distance*ray.getDirection();		
	ic.t=distance;

	//find dominat axis
	
	real p[2];
	p[0]= _dominant==0?(point[1]-_v1[1]):(point[0]-_v1[0]);
	p[1]= _dominant==2?(point[1]-_v1[1]):(point[2]-_v1[2]);


	real u = p[1]*_ud1  + p[0]*_ud2;
	if(u<-eps){//not hit
		ic.t = -1;	
		return ic;		
	}


	real v = p[1]* _vd1 + p[0]*_vd2;
	//ensure u,v,w \in [0,1]
	if( v<-eps or u+v>1+eps){ // not hit
		ic.t = -1;	
		return ic;	
	}

	//calculate correct color
	ic.mat = getMaterial();	
	if(not _normalsSet )
		ic.normal = _normal;
	else
		ic.normal = u*_normals[1] + v*_normals[2] + (1-u-v)*_normals[0];
		ic.normal.normalize();
		//normals have to be initialiased with somethng useful!
		ic.px =  u*_px[1] + v*_px[2] + (1-u-v)*_px[0];
		ic.py =  u*_py[1] + v*_py[2] + (1-u-v)*_py[0];
	return ic;
}


void Triangle::setTextureCoords(std::string str){
	std::istringstream iss(str);
	std::string sub = "";
	if(iss){//parse  x1     
		iss >> sub;
		_px[0] = atof(sub.c_str());
	} else { std::cout<<"Warning: no 1th element in constructor argument for Vector3. arg: "
		<<str<<". Using 0."<<std::endl; }		
	if(iss){//parse  y1     
		iss >> sub;
		_py[0] = atof(sub.c_str());
	}else{	std::cout<<"Warning: no 2th element in constructor argument for Vector3. arg: "
		<<str<<". Using 0."<<std::endl; }
	if(iss){//parse x2      
		iss >> sub;
		_px[1] = atof(sub.c_str());
	}else{	std::cout<<"Warning: no 3th element in constructor argument for Vector3. arg: "
		<<str<<". Using 0."<<std::endl; }
	if(iss){//parse y2        
		iss >> sub;
		_py[1] = atof(sub.c_str());
	}else{	std::cout<<"Warning: no 4th element in constructor argument for Vector3. arg: "
		<<str<<". Using 0."<<std::endl; }
	if(iss){//parse x3       
		iss >> sub;
		_px[2] = atof(sub.c_str());
	}else{	std::cout<<"Warning: no 5th element in constructor argument for Vector3. arg: "
		<<str<<". Using 0."<<std::endl; }
	if(iss){//parse y3        
		iss >> sub;
		_py[2] = atof(sub.c_str());
	}else{	std::cout<<"Warning: no 6th element in constructor argument for Vector3. arg: "
		<<str<<". Using 0."<<std::endl; }

}

