#include "KDTree.h"

#include <iostream>
#include <string>
#include <vector>

void KDTree::traverse(const Node *n, int level) const{
	if(level == 0)
		std::cout << "Starting with root:" << std::endl;

	std::string s = "";
	for(int i = 0; i< level; ++i)
		s+='\t';

	//print current data:
	std::cout<<s<<"split: "<<n->split<<std::endl;
	std::cout<<s<<"is a leaf: "<<(n->isLeaf?"yes":"no" )<<std::endl;
	if(n->isLeaf){//print data
		if(n->triags){
			std::cout<<s<<"data: ";
			for (unsigned int i =0; i< n->triags->size(); ++i)
				std::cout<< (n->triags->at(i)) <<" ";
			std::cout<<"\n"<<std::endl;
		}else{
			std::cout<<s<<"no data contained\n"<< std::endl;
		}
	}else{//recursive calls
		std::cout<<s<<"right:"<<std::endl;
		traverse(n->left+1,level+1);//left
		std::cout<<s<<"left:"<<std::endl;
		traverse(n->left, level+1);//right
	}
}

int KDTree::addTriangle(Triangle t){
	if (_triangleStorage.empty()){
		//init bounds with the dimension of the first point in the triangle
		_bounds[0][0] = _bounds[0][1] = t.getV1()[0];
		_bounds[1][0] = _bounds[1][1] = t.getV1()[1];
		_bounds[2][0] = _bounds[2][1] = t.getV1()[2];
	}

	//set the real max,min positions
	for(int dim = 0; dim <3; ++dim){
		_bounds[dim][0] = std::min( _bounds[dim][0],  t.getV1()[dim]);
		_bounds[dim][1] = std::max( _bounds[dim][1],  t.getV1()[dim]);
		_bounds[dim][0] = std::min( _bounds[dim][0],  t.getV2()[dim]);
		_bounds[dim][1] = std::max( _bounds[dim][1],  t.getV2()[dim]);
		_bounds[dim][0] = std::min( _bounds[dim][0],  t.getV3()[dim]);
		_bounds[dim][1] = std::max( _bounds[dim][1],  t.getV3()[dim]);

	}

	_triangleStorage.push_back(t);
	return _triangleStorage.size()-1;

}

void KDTree::init(){
	//TODO build KDTree with given data
}


IntersectionCompound KDTree::getIntersection(const Ray& r) const{
	//TODO: use the tree to find the nearest intersection, this is too pricy O(n)
	IntersectionCompound inter;
	real nearest = std::numeric_limits<real>::infinity();
	foreach(const Triangle& triag, _triangleStorage){
		IntersectionCompound tmp = triag.getIntersection(r);
		//test if there is a ral hit AND if the hit triangle is nearest than the nearest so far
		if( tmp.t > 0 and tmp.t < nearest)
			inter = tmp;
	}
	return 	inter;
}

/*
KDTree::KDTree(float someData){
	_root = new Node(false, someData);
	//new children
	_root->left = new Node[2];
		//init children
		_root->left[0].isLeaf = false;
		_root->left[0].split = -3;
		_root->left[1].isLeaf = true;
		_root->left[1].split = 4;
		_root->left[1].triags = new std::vector<Triangle>();
		_root->left[1].triags->push_back(1.);
		_root->left[1].triags->push_back(3.);
		_root->left[1].triags->push_back(5.);
		//new children
		_root->left[0].left = new Node[2];
			//init children
			_root->left[0].left[0].isLeaf = true;
			_root->left[0].left[0].split = 2;
			_root->left[0].left[0].triags = 0;
			_root->left[0].left[1].isLeaf = true;
			_root->left[0].left[1].split = -2;
			_root->left[0].left[1].triags = new std::vector<Triangle>();
			_root->left[0].left[1].triags->push_back(2);
}
*/

/*
int main(){

	KDTree tree(12);
	tree.traverse();

}
*/

