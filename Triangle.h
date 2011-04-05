#pragma once
#include "Obstacle.h"

class Triangle: public Obstacle{

	private:
		Vector3  _v1,_v2,_v3;
		Vector3 _normal;
		Vector3 _normals[3];
		bool _normalsSet;		
				

		//neede for the intersection test
		real _b[2],_c[2];
		real _div1,_div2;
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

		//methods
		const Vector3& getTriangleNormal()const{return _normal;}
		void addNormal(unsigned int pos, const Vector3 &normal){
			_normals[pos] += normal;
		}

		Vector3* getVertex1(){return &_v1;} 			
		Vector3* getVertex2(){return &_v2;} 			
		Vector3* getVertex3(){return &_v3;} 			

		void normalizeNormals(){
			_normals[0].normalize();_normals[1].normalize();_normals[2].normalize();
			_normalsSet = true;
		}
};
