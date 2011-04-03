#pragma once

#include <valarray>
#include <iostream>
#include "common.h"


class Vector3{

private:
	std::valarray<real> _data;

public:
	//constructors
	Vector3(real r1 = 0, real r2 = 0, real r3 = 0):_data(0.0,3){_data[0]=r1; _data[1]=r2; _data[2]=r3; }
	Vector3(const Vector3 &vec):_data(vec._data){}	

	//methods
	inline real length() const{ return sqrt( (*this).dot(*this) ); }
	inline real dot(const Vector3& vec) const { 	
		std::valarray<real> va = _data * vec._data;
		return va[0]+va[1]+va[2];
	}
	inline Vector3 cross(const Vector3& vec) const{
		Vector3 result;
		result[0] = _data[1]*vec._data[2] - _data[2]*vec._data[1];
		result[1] = _data[2]*vec._data[0] - _data[0]*vec._data[2];
		result[2] = _data[0]*vec._data[1] - _data[1]*vec._data[0];
		return result;
	}


	inline Vector3& normalize(){ _data/=length();return *this; }

	
	//operators
	//get AND set
	inline real& operator[](const int n){ return _data[n]; } 	
	inline const real& operator[](const int n) const {return _data[n];}
	//valarray kann : +,-,*,/,sqrt,
	//abs, log, log10, pow, sin, cos, sqrt
	//operators
	
	inline Vector3 operator+(const Vector3& vec) const{
		Vector3 newVec =  *this;
		newVec+=vec;
		return newVec;
	}

	inline Vector3& operator+=(const Vector3& vec){
		_data += vec._data;
		return *this;	
	}

	inline Vector3 operator-(const Vector3& vec) const{
		Vector3 newVec =  *this;
		newVec-=vec;
		return newVec;	
	}
	inline Vector3& operator-=(const Vector3& vec){
		_data -= vec._data;
		return *this;	
	}

	inline Vector3 operator*(const Vector3& vec) const{
		Vector3 newVec =  *this;
		newVec*=vec;
		return newVec;
	}

	inline Vector3 operator*(real a) const{
		Vector3 newVec =  *this;
		newVec*=a;
		return newVec;
	}
	inline Vector3& operator*=(const Vector3& vec){
		_data *= vec._data;
		return *this;	
	}
	inline Vector3& operator*=(real a){
		_data *= std::valarray<real>(a, 3);
		return *this;	
	}
	
	inline Vector3 operator/(const Vector3& vec) const{
		Vector3 newVec =  *this;
		newVec/=vec;
		return newVec;
	}
	inline Vector3 operator/(real a) const{
		Vector3 newVec =  *this;
		newVec/=a;
		return newVec;
	}
	inline Vector3& operator/=(const Vector3& vec){
		_data /= vec._data;
		return *this;	
	}
	inline Vector3& operator/=(real a){
		_data /= std::valarray<real>(a, 3);;
		return *this;	
	}

};


//global operators
inline Vector3 operator*(real a, const Vector3& vec){
	Vector3 newVec = vec;
	newVec*=a;
	return newVec;
}



//streamoperator
inline std::ostream &operator<<(std::ostream &stream, const Vector3 vec){
	Vector3 v = vec;//used because operator[] is obviously not a const function
	stream << "( "<<v[0]<<", "<<v[1]<<", "<<v[2]<<" )";
 	return stream; // must return stream
}
