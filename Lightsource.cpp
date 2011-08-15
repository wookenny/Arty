#include "Lightsource.h"

#include <random>

void Lightsource::sqaureDirectionFiller(const Vector3& pos,int num, std::vector<Vector3> &vec) const{
	//some little randomization needed here => less banding effects
	std::mt19937_64 e(0/*seed*/);
	std::uniform_real_distribution<double> d(-0.5,0.5);

	real offset = _size/(num-1);
	Vector3 start = _position - (_size/2.)*(_direction1+_direction2);
	for(int i = 0; i<num; ++i)
		for(int j = 0; j<num; ++j){
			real x = i+d(e);
			real y = j+d(e);
			vec.push_back( start + (x*offset)*_direction1 + (y*offset)*_direction2 - pos );
		}
}

void Lightsource::getDirectionsToLight( const Vector3& pos, int num, std::vector<Vector3> &vec) const{
	switch(_type){
		case Type::Point:
			vec.push_back( getDirectionToLight(pos) );
			break;

		case Type::Square:
			sqaureDirectionFiller(pos,num,vec);
			break;

		case Type::Disk:
			break;

		case Type::Ball:
			break;

	}

}

