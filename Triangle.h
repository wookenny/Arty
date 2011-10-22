#pragma once
#include "Obstacle.h"
#include <cstdlib>
#include "AABB-triangle-overlap.h"

class Triangle: public Obstacle{

	private:
		Vector3  _v1,_v2,_v3;
		Vector3 _normal;
		Vector3 _normals[3];
		real _px[3]; //texture coordinates
		real _py[3]; //texture coordinates
		bool _normalsSet;


		//neede for the intersection test
		//no c, no b, no div
		//real _b[2],_c[2];
		//real _div1,_div2;
		real _ud1, _ud2, _vd1, _vd2;

		int _dominant;

		int getDominantAxis() const{
			return (abs(_normal[0])>abs(_normal[1]))?
				(abs(_normal[0])>abs(_normal[2])?0:2):
				(abs(_normal[1])>abs(_normal[2])?1:2);
		}


	public:
		IntersectionCompound getIntersection(const Ray&) const;

			//c_strs
		Triangle( const Vector3& v1, const Vector3& v2, const Vector3& v3);


		Vector3 getV1() const{return _v1;}
		Vector3 getV2() const{return _v2;}
		Vector3 getV3() const{return _v3;}

		void setNormals(const Vector3  n1, const Vector3 n2, const Vector3 n3){
			_normals[0] = n1;_normals[1] = n2;_normals[2] = n3;
			_normals[0].normalize();_normals[1].normalize();_normals[2].normalize();
			_normalsSet = true;
		}

		void setTextureCoords(std::string coords);
		bool overlapAABB(const Vector3 &boxMin, const Vector3 &boxMax) const{ 
					return triBoxOverlap(boxMin,boxMax,this);}

};

//streamoperator
inline std::ostream &operator<<(std::ostream &stream, const Triangle t){
	stream << t.getV1()<<", "<<t.getV2()<<", "<<t.getV3();
 	return stream; // must return stream
}

