#pragma once

#include <vector>
#include "Triangle.h"
#include "Obstacle.h"

class KDTree{
	private:
		//Node structure for the tree
		struct Node{
			bool isLeaf; //1 byte
			float split; //4 byte
			union{ //8 byte
				Node* left;
				std::vector<Triangle*> *triags;
			}; // => in total: 13 byte, so it will take 16 byte
			//default node constr.
			Node(bool leaf = false, float s = 0.f):isLeaf(leaf),split(s),left(0)/* triags implicitly to 0!*/{}
			//node destructor, depends heavily on the right initialization
			~Node(){
				if(isLeaf and triags)
					delete triags;
				else
					delete [] left;
			}

		};

		Node *_root;
		std::vector<Triangle> _triangleStorage;
		Vector3 _boxMin,_boxMax;

		
		static const int COST_INTERSECTION = 10;
		static const int COST_TRAVERSAL = 2;

		/*traversal method. this has to be altered */
		void traverse(const Node *n, int level) const;
		void _buildkdtree( Node* node, std::vector<Triangle*> containedTrigs,
					const Vector3 &boxMin, const Vector3 &boxMax, int level);
		real _getOptimalSplitPosition(std::vector<Triangle*> &containedTrigs, 
					const Vector3 &boxMin, const Vector3 &boxMax, int level);

		inline real _area(const Vector3 &boxMin, const Vector3 &boxMax){
		  //area = 2 * ((width * height) + (height * depth) + (depth * width))
		      return 2*(     	(boxMax[0]-boxMin[0])*(boxMax[1]-boxMin[1])	 
				      + (boxMax[1]-boxMin[1])*(boxMax[2]-boxMin[2])
				      + (boxMax[2]-boxMin[2])*(boxMax[0]-boxMin[0]) );
		}
		
	public:


		KDTree():_root(0),_triangleStorage(){};
		~KDTree(){delete _root;}//this calls a recursive call for all of the rest

		int addTriangle(Triangle t);
		void init();
		void traverse() const { traverse(_root, 0); }

		IntersectionCompound getIntersection(const Ray&) const;
};

