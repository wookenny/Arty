#pragma once
#include "Vector3.h"
#include "Color.h"
#include "common.h"

class Lightsource{

private:
	Vector3 _position;
	real _intensity;
	Color _color;
	//TODO: fuer weiche schatten aufbohren
	//Obstacle _geometry;

public:
	//constructors
	Lightsource(const Vector3 p, const real i=1, Color c = Color(1,1,1)):_position(p),_intensity(i),_color(c){}
	Lightsource(const Lightsource& l):_position(l._position),_intensity(l._intensity),_color(l._color){}	

	//getter
	const Vector3& getPosition() const{ return _position; }
	real getIntensity() const{ return _intensity; }
	const Color& getColor() const{ return _color; } 


	Vector3 getDirectionToLight( const Vector3& pos ) const{//not a normalized vector -> shadow if intersection parameter < 1.0
		return (_position-pos);
	}	

};
