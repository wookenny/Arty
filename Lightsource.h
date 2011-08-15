#pragma once
#include "Vector3.h"
#include "Color.h"
#include "common.h"

class Lightsource{

public:
	enum class Type{
			Point,
			Square,
			Disk,
			Ball
		};

private:
	Vector3 _position;
	real _intensity;
	Color _color;
	real _size;
	Type _type;


public:



	//constructors
	Lightsource(const Vector3 p, const real i=1, Color c = Color(1,1,1)):
							_position(p),_intensity(i),_color(c),_size(1),_type(Type::Point){}
	Lightsource(const Lightsource& l):
			_position(l._position),_intensity(l._intensity),_color(l._color),_size(l._size),_type(l._type){}

	//getter
	const Vector3& getPosition() const{ return _position; }
	real getIntensity() const{ return _intensity; }
	const Color& getColor() const{ return _color; }
	real getSize() const{ return _size;}
	Type getType() const{ return _type;}

	Vector3 getDirectionToLight( const Vector3& pos ) const{//not a normalized vector -> shadow if intersection parameter < 1.0
		return (_position-pos);
	}

};

