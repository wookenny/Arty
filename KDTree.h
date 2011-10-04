#pragma once

#include <vector>
//#include "Triangle.h"
typedef long Triangle;


class KDTree{

	private:
		struct Node{
			bool isLeaf; //1 byte
			float split; //4 byte
			union{ //8 byte
				Node* left;
				std::vector<Triangle> *triags;
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


		/*traversal method. this has to be altered */
		void traverse(const Node *n, int level) const;

	public:
		KDTree() = delete;//no default constr!
		KDTree(float someData);
		~KDTree(){delete _root;}//this calls a recursive call for all of the rest
		void traverse(){ traverse(_root, 0); }

};


