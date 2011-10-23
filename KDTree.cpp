#include "KDTree.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

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
		_boxMin[0] = _boxMax[0] = t.getV1()[0];
		_boxMin[1] = _boxMax[1] = t.getV1()[1];
		_boxMin[2] = _boxMax[2] = t.getV1()[2];
	}

	//set the real max,min positions
	for(int dim = 0; dim <3; ++dim){
		_boxMin[dim] = std::min( _boxMin[dim],  t.getV1()[dim]);
		_boxMax[dim] = std::max( _boxMax[dim],  t.getV1()[dim]);
		
		_boxMin[dim] = std::min( _boxMin[dim],  t.getV2()[dim]);
		_boxMax[dim] = std::max( _boxMax[dim],  t.getV2()[dim]);

		_boxMin[dim] = std::min( _boxMin[dim],  t.getV3()[dim]);
		_boxMax[dim] = std::max( _boxMax[dim],  t.getV3()[dim]);
	}

	_triangleStorage.push_back(t);
	return _triangleStorage.size()-1;

}

void KDTree::init(){

	_root = new Node();
	std::vector<Triangle*> contained;
	for(unsigned int i=0; i<_triangleStorage.size(); ++i)
		contained.push_back(&_triangleStorage.at(i));
	
	
	std::cout<<"initialising kd-tree node with bounds:"
		 << "["<<_boxMin[0]<<";" << _boxMax[0]<<"]x["
		 << _boxMin[1]<<";" << _boxMax[1]<<"]x["
		 << _boxMin[2]<<";" << _boxMax[2]<<"]"<<std::endl;
	std::cout<<" triangles: "<<contained.size()<<std::endl;
	
	_buildkdtree(_root, contained, _boxMin, _boxMax, 0);
}




real KDTree::_getOptimalSplitPosition(std::vector<Triangle*> &containedTrigs, 
					const Vector3 &boxMin, const Vector3 &boxMax, int level){
	// area = 2 * ((width * height) + (height * depth) + (depth * width))
        //cost = Ctraversal + area * prims * Cintersect
  
	//no split: 
	real area = _area(boxMin, boxMax);
	real no_split = area*containedTrigs.size()*COST_INTERSECTION ;

	real split_pos = boxMin[level%3];
	real best_cost = no_split;
	  
	std::vector<real> positions;
	foreach(Triangle *t, containedTrigs){
		positions.push_back(t->getV1()[level%3]);
		positions.push_back(t->getV2()[level%3]);
		positions.push_back(t->getV3()[level%3]);
	}

	
	//move the splitting position
	//it starts with all max positions and in the right level the min position
	Vector3 split_position = boxMax;
	split_position[level%3] = boxMin[level%3];
	
	foreach(real pos, positions){
	    //adjust the center and size of the boxes
	    split_position[level%3] = pos;
 
	    //count triangels in right and left set
	    real triags_left = 0, triags_right = 0;
	    foreach(const Triangle *t, containedTrigs){
		  if( t->overlapAABB( boxMin, split_position) )
		      ++triags_left;
		  if( t->overlapAABB( split_position, boxMax) ) 
		      ++triags_right;
		  
	    }	
	    
	    //calculate areas
	    real area_left = _area(boxMin, split_position), area_right = _area(split_position, boxMax);
	    
	    //cost = COST_TRAVERSAL + cost left +cost right
	    real cost = COST_TRAVERSAL + 
			area_left*triags_left*COST_INTERSECTION +
			area_right*triags_right*COST_INTERSECTION;//cost left
	    if (cost < best_cost){//better split position?
		cost = best_cost;
		split_pos = pos;
	    }
	}
	return split_pos;
  
}


void KDTree::_buildkdtree(Node* node, std::vector<Triangle*> containedTrigs,
					const Vector3 &boxMin, const Vector3 &boxMax, int level){
	
	
	//splitpos: splitpos < bounds -> do not split, single node
	//IF IS LEAF: NO SPLITTING
	real splitpos = _getOptimalSplitPosition(containedTrigs,boxMin,boxMax,level);

	//is the new node a leaf?
	//do not split such that the resulting cube has a volumue of zero
	if(splitpos-eps <=  boxMin[level%3]  or splitpos+eps >=  boxMax[level%3]){
		node->isLeaf = true;
		node->split = 0.f;
		node->triags = new std::vector<Triangle*>;
		foreach(Triangle* t, containedTrigs)
			node->triags->push_back(t);
		std::cout<<"building leaf at level "<<level<<" with "<<containedTrigs.size()<<" triangles"
		 <<" and bounds: "<< "["<<boxMin[0]<<";" << boxMax[0]<<"]x["
		 << boxMin[1]<<";" << boxMax[1]<<"]x["
		 << boxMin[2]<<";" << boxMax[2]<<"]"<<std::endl;
		return;
	}
	
	
	//find all correct triangles
	std::vector<Triangle*> contained_left, contained_right;
	Vector3 split_position = boxMax;
	split_position[level%3] = splitpos;

	//assign triangles to sides(can be on both)
	Triangle *t;
	foreach(t, containedTrigs){
		if ( t->overlapAABB(boxMin, split_position) ) 
			contained_left.push_back(t);
		if ( t->overlapAABB(split_position, boxMax) )
			contained_right.push_back(t);
	}

	//set correct date for the current node:
	//isLeaf, split and left or triags
	node->isLeaf = false;
	node->split = splitpos;
	node->triags = new std::vector<Triangle*>;
	foreach(Triangle* t, containedTrigs)
		node->triags->push_back(t);	

	std::cout<< "Spltting node at level "<<level<<" at position "<<splitpos <<std::endl;
	node->left = new Node[2];	
	Node *leftnode = node->left;
	Node *rightnode = node->left+1;
	_buildkdtree( leftnode,  contained_left, boxMin, split_position, level+1  );
	_buildkdtree( rightnode, contained_right, split_position, boxMax, level+1 );
}


void KDTree::_getIntersection(const Vector3 &origin, const Vector3 &direction, IntersectionCompound &inter) const{
	//find position in kdtree

	//check for nearest intersection

	//if intersection => return inter.

	//if no intersection => extend oring to next border, add EPS and do a recursive call,
	//no next border => end
	
}		

IntersectionCompound KDTree::getIntersection(const Ray& r) const{
	//TODO: use the tree to find the nearest intersection, this is too pricy O(n)
	IntersectionCompound inter;
	real nearest = std::numeric_limits<real>::infinity();
	//_getIntersection( r.getOrigin(), r.getDirection(), inter);
	///*
	foreach(const Triangle& triag, _triangleStorage){
		IntersectionCompound tmp = triag.getIntersection(r);
		//test if there is a real hit AND if the hit triangle is nearer than the nearest so far
		if( tmp.t > 0 and tmp.t < nearest)
			inter = tmp;
	}
	//*/
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
		_root->left[1].triags = new std::vector<*Triangle>();
		//new children
		_root->left[0].left = new Node[2];
			//init children
			_root->left[0].left[0].isLeaf = true;
			_root->left[0].left[0].split = 2;
			_root->left[0].left[0].triags = 0;
			_root->left[0].left[1].isLeaf = true;
			_root->left[0].left[1].split = -2;
			_root->left[0].left[1].triags = new std::vector<*Triangle>();

}
*/

/*
int main(){

	KDTree tree(12);
	tree.traverse();

}
*/

