#include "Vector3.h"
#include <cmath>
#include <sstream>
#include <cstdlib>

Vector3::Vector3(const std::string &str):_data(0.0,3){
	//parse x,y,z
	std::istringstream iss(str);
	std::string sub = "";
	if(iss){//parse x        
		iss >> sub;
		_data[0] = atof(sub.c_str());
	} else { std::cout<<"Warning: no 1th element in constructor argument for Vector3. arg: "
		<<str<<". Using 0."<<std::endl; }		
	if(iss){//parse y        
		iss >> sub;
		_data[1] = atof(sub.c_str());
	}else{	std::cout<<"Warning: no 2th element in constructor argument for Vector3. arg: "
		<<str<<". Using 0."<<std::endl; }
	if(iss){//parse z        
		iss >> sub;
		_data[2] = atof(sub.c_str());
	}else{	std::cout<<"Warning: no 3th element in constructor argument for Vector3. arg: "
		<<str<<". Using 0."<<std::endl; }
}	

//operators

