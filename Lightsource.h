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
	Vector3 _direction1;
	Vector3 _direction2;

	//some helper functions:
	void sqaureDirectionFiller(const Vector3& pos,int num, std::vector<Vector3> &vec) const;

public:



	//constructors
	Lightsource(const Vector3 p, const real i=1, Color c = Color(1,1,1)):
							_position(p),_intensity(i),_color(c),
							_size(1),_type(Type::Point),_direction1(Vector3(1,0,0)),
							_direction2(Vector3(0,1,0)){}
	Lightsource(const Lightsource& l):
			_position(l._position),_intensity(l._intensity),_color(l._color),
			_size(l._size),_type(l._type),_direction1(l._direction1),
			_direction2(l._direction2){}

	//setter
	void setType(Type type){_type = type;}
	void setSize(real s){_size = s;}
	void setDirection1(Vector3 dir){_direction1 = dir;}
	void setDirection2(Vector3 dir){_direction2 = dir;}

	//getter
	const Vector3& getPosition() const{ return _position; }
	real getIntensity() const{ return _intensity; }
	const Color& getColor() const{ return _color; }
	real getSize() const{ return _size;}
	Type getType() const{ return _type;}

	Vector3 getDirectionToLight( const Vector3& pos ) const{//not a normalized vector -> shadow if intersection parameter < 1.0
		return (_position-pos);
	}

	//used to cast soft shadows. Create rays that hit the light for some point on the surface of the light
	//The parameter num controls the number of rays. Most likely, there will be num^2 directions afterwards.
	void getDirectionsToLight( const Vector3& pos, int num, std::vector<Vector3> &vec) const;

};

