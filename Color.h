#pragma once
#include "common.h"

#include <string>
#include <algorithm>
#include <iostream>

class Color{


private:
	real _data[3];//stores RGB values

public:

//con'strs
	Color(const std::string &str);
	Color(real r=0, real g=0, real b=0){ _data[0]=r; _data[1]=g; _data[2]=b; }
	Color(const Color &c){_data[0] = c._data[0]; _data[1] = c._data[1]; _data[2] = c._data[2];}
	

	//getter
	real getRed() const{ return _data[0];}
	real getGreen() const{ return _data[1];}
	real getBlue() const{ return _data[2];}
	real sum() const{ return _data[0]+_data[1]+_data[2]; }
	real max() const{ return std::max(_data[0],std::max(_data[1],_data[2]));}

	//setter
	void setRed(real red){ _data[0] = red;}
	void setGreen(real green){ _data[1] = green;}
	void setBlue(real blue){ _data[2] = blue;}

	// misc methods	
	std::string toString() const;
	Color& gammaCorrection(real gamma);
	Color& exposureCorrection(); //map all colors to [0,1]^3

	//operators
	Color operator+(const Color& c) const;
	Color& operator+=(const Color& c);

	Color operator-(const Color& c) const;
	Color& operator-=(const Color& c);

	Color operator*(const Color& c) const;
	Color operator*(real a) const;
	Color& operator*=(const Color& c);
	Color& operator*=(real a);
	
	Color operator/(const Color& c) const;
	Color operator/(real a) const;
	Color& operator/=(const Color& c);
	Color& operator/=(real a);
	
};

//global operators
Color operator*(real a, const Color& c);

//stream operator
std::ostream &operator<< (std::ostream &stream, const Color& c);
