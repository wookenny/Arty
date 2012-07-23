#pragma once
#include "Obstacle.h"
#include "Triangle.h"

/**
structure:
two triangles with this numbering:

1----2
|\ t2|
| \  |
|t1 \|
4----3
***/
class Quad: public Obstacle{

	private:
		Triangle _t1,_t2;

		
	public:
		IntersectionCompound getIntersection(const Ray&) const{
			//TODO: intersection for both triangles
		}

		//c_strs
		Quad( const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4){
			//TODO init bot triangles
		};


		Vector3 getV1() const{return _t1.getV1();}
		Vector3 getV2() const{return _t1.getV2();}
		Vector3 getV3() const{return _t1.getV3();}
		Vector3 getV4() const{return _t2.getV3();}

		void setNormals(const Vector3  n1, const Vector3 n2, const Vector3 n3, const Vector3 n4){
			//TODO call it for triangles
		}

		void setTextureCoords(std::string coords){
			//set textures coords for both triags
		}
		
		//TODO: do this wit both triangles AND combine with AND or OR
		bool overlapAABB(const Vector3 &boxMin, const Vector3 &boxMax) const{ 
					return triBoxOverlap(boxMin-Vector3(eps,eps,eps),boxMax+Vector3(eps,eps,eps),_t1/*this*/)
						or triBoxOverlap(boxMin-Vector3(eps,eps,eps),boxMax+Vector3(eps,eps,eps),_t2);}

};

//streamoperator
inline std::ostream &operator<<(std::ostream &stream, const Quad q){
	stream << q.getV1()<<", "<<q.getV2()<<", "<<q.getV3()", "<<q.getV4();
 	return stream; // must return stream
}

