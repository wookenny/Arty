#include "Color.h"

#include <sstream>
#include <cmath>
#include <cstdlib>
#include <iostream>


Color::Color(const std::string &str){
	_data[0]=_data[1]=_data[2]=0;
	//parse r,g,b
	std::istringstream iss(str);
	std::string sub = "";
	if(iss){//parse x        
		iss >> sub;
		_data[0] = atof(sub.c_str());
	} else { std::cout<<"Warning: no 1th element in constructor argument for Color. arg: "
		<<str<<". Using 0."<<std::endl; }		
	if(iss){//parse y        
		iss >> sub;
		_data[1] = atof(sub.c_str());
	}else{	std::cout<<"Warning: no 2th element in constructor argument for Color. arg: "
		<<str<<". Using 0."<<std::endl; }
	if(iss){//parse z        
		iss >> sub;
		_data[2] = atof(sub.c_str());
	}else{	std::cout<<"Warning: no 3th element in constructor argument for Color. arg: "
		<<str<<". Using 0."<<std::endl; }
}	


std::string Color::toString() const{
	std::stringstream sstr;
	sstr<<"("<<_data[0]<<", "<<_data[1]<<", "<<_data[2]<<")";
	return sstr.str();	
}

Color& Color::gammaCorrection(real gamma){
	_data[0] = pow(_data[0],(1./gamma));
	_data[1] = pow(_data[1],(1./gamma));
	_data[2] = pow(_data[2],(1./gamma));
	return *this;
}

Color& Color::exposureCorrection(){
	_data[0] = 1.0 - exp(-_data[0]);
	_data[1] = 1.0 - exp(-_data[1]);
	_data[2] = 1.0 - exp(-_data[2]);
	return *this;
}

Color Color::operator+(const Color& c) const{
	Color newColor =  *this;
	newColor+=c;
	return newColor;
}

Color Color::operator-(const Color& c) const{
	Color newColor = *this;
	newColor-=c;
	return newColor;
}

Color Color::operator*(const Color& c) const{
	Color newColor = *this;
	newColor*=c;
	return newColor;
}

Color Color::operator*(real a) const{
	Color newColor = *this;
	newColor*=a;
	return newColor;
}

Color Color::operator/(const Color& c) const{
	Color newColor = *this;
	newColor/=c;
	return newColor;
}

Color Color::operator/(real a) const{
	Color newColor = *this;
	newColor/=a;
	return newColor;
}

Color& Color::operator+=(const Color& c){
	_data[0]+=c._data[0];	
	_data[1]+=c._data[1];	
	_data[2]+=c._data[2];
	return *this;	
}

Color& Color::operator-=(const Color& c){
	_data[0]-=c._data[0];	
	_data[1]-=c._data[1];	
	_data[2]-=c._data[2];	
	return *this;
}

Color& Color::operator*=(const Color& c){
	_data[0]*=c._data[0];	
	_data[1]*=c._data[1];	
	_data[2]*=c._data[2];	
	return *this;
}

Color& Color::operator*=(real a){
	_data[0]*=a;
	_data[1]*=a;
	_data[2]*=a;
	return *this;
}

Color& Color::operator/=(const Color& c){
	_data[0]/=c._data[0];	
	_data[1]/=c._data[1];	
	_data[2]/=c._data[2];	
	return *this;
}

Color& Color::operator/=(real a){
	_data[0]/=a;
	_data[1]/=a;
	_data[2]/=a;
	return *this;
}



//global Operator
Color operator*(real a, const Color& c){
	Color newColor = c;
	newColor*=a;
	return newColor;
}

std::ostream &operator<<(std::ostream &stream, Color c)
{
  stream<<c.toString();
  return stream;
}
