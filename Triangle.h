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

		
		void setNormals(const Vector3  n1, const Vector3 n2, const Vector3 n3){
			_normals[0] = n1;_normals[1] = n2;_normals[2] = n3;
			_normals[0].normalize();_normals[1].normalize();_normals[2].normalize();
			_normalsSet = true;
		}
};
