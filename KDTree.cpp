#include "KDTree.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>


void KDTree::traverse(const Node *n, int level) const{
	if(level == 0)
		std::cout << "Starting with root:" << std::endl;

	std::string s = "";
	for(int i = 0; i< level; ++i)
		s+='\t';

	//print current data:
	std::cout<<s<<"split: "<<n->split<<std::endl;
	std::cout<<std::boolalpha;
	std::cout<<s<<"is a leaf: "<<(n->isLeaf)<<std::endl;
	if(n->isLeaf){//print data
		if(n->triags != nullptr){
			std::cout<<s<<"data: ";
			for (unsigned int i =0; i< n->triags->size(); ++i)
				std::cout<< (n->triags->at(i)) <<" ";
			std::cout<<"\n"<<std::endl;
		}else{
			std::cout<<s<<"no data contained\n"<< std::endl;
		}
	}else{//recursive calls
		std::cout<<s<<"right:"<<std::endl;
		traverse(&(n->left[1]),level+1);//left
		std::cout<<s<<"left:"<<std::endl;
		traverse(&(n->left[0]), level+1);//right
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

	_root = std::move(std::unique_ptr<Node>(new Node()));
	std::vector<Triangle*> contained;
	for(unsigned int i=0; i<_triangleStorage.size(); ++i)
		contained.push_back(&_triangleStorage.at(i));

	std::cout<<"initialising kd-tree node with bounds:"
		 << "["<<_boxMin[0]<<";" << _boxMax[0]<<"]x["
		 << _boxMin[1]<<";" << _boxMax[1]<<"]x["
		 << _boxMin[2]<<";" << _boxMax[2]<<"]"<<std::endl;
	std::cout<<" triangles: "<<contained.size()<<std::endl;
	
	_buildkdtree(_root.get(), contained, _boxMin, _boxMax, 0);
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
	for(Triangle *t: containedTrigs){
		positions.push_back(t->getV1()[level%3]);
		positions.push_back(t->getV2()[level%3]);
		positions.push_back(t->getV3()[level%3]);
	}

	
	//move the splitting position
	//it starts with all max positions and in the right level the min position
	Vector3 split_position = boxMax;
	split_position[level%3] = boxMin[level%3];
	
	for(real pos: positions){
	    //adjust the center and size of the boxes
	    split_position[level%3] = pos;
 
	    //count triangels in right and left set
	    real triags_left = 0, triags_right = 0;
	    for(const Triangle *t: containedTrigs){
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
		node->triags = std::move(std::unique_ptr<std::vector<Triangle*> >(new std::vector<Triangle*>));
		for(Triangle* t: containedTrigs)
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
	for(Triangle *t: containedTrigs){
		if ( t->overlapAABB(boxMin, split_position) ) 
			contained_left.push_back(t);
		if ( t->overlapAABB(split_position, boxMax) )
			contained_right.push_back(t);
	}

	//set correct date for the current node:
	//isLeaf, split and left or triags
	node->isLeaf = false;
	node->split = splitpos;
	node->triags = std::move(std::unique_ptr<std::vector<Triangle*> >(new std::vector<Triangle*>));
	for(Triangle* t: containedTrigs)
		node->triags->push_back(t);	

	std::cout<< "Spltting node at level "<<level<<" at position "<<splitpos <<std::endl;
	
	node->left = std::move(std::unique_ptr<Node[]>(new Node[2]));	
	Node *leftnode  = &node->left[0];
	Node *rightnode = &node->left[1];
	_buildkdtree( leftnode,  contained_left, boxMin, split_position, level+1  );
	_buildkdtree( rightnode, contained_right, split_position, boxMax, level+1 );
}


void KDTree::_getIntersection(Vector3 &position, const Ray &r, IntersectionCompound &inter) const{
	//TODO: delete this bad version 	
	/*
	real nearest = std::numeric_limits<real>::infinity();
	foreach(const Triangle& triag, _triangleStorage){
		IntersectionCompound tmp = triag.getIntersection(r);
		//test if there is a real hit AND if the hit triangle is nearer than the nearest so far
		if( tmp.t > 0 and tmp.t < nearest){
			inter = tmp;
			nearest = tmp.t;		
		}
	}
	 */


	if(    position[0]-2*eps >= _boxMax[0] or position[1]-2*eps >= _boxMax[1] or position[2]-2*eps >= _boxMax[2]
	    or position[0]+2*eps <= _boxMin[0] or position[1]+2*eps <= _boxMin[1] or position[2]+2*eps <= _boxMin[2]){
		inter.t  = -1;//no intersection, set dist to -1
		return;	
	}	

	//find position in kdtree and store the bounds
	Node *leaf = _root.get();
	int level = 0;
	Vector3 boxMax = _boxMax;
	Vector3 boxMin = _boxMin;
	while(not (leaf->isLeaf)){
		//to right or to left?
		if(position[level%3] <= leaf->split){
			leaf = &leaf->left[0];
			boxMax[level%3] = leaf->split;	
		}else{
		 	leaf = &leaf->left[1];
			boxMin[level%3] = leaf->split;
		}	
		++level;
	}

	//check for nearest intersection
	real nearest = std::numeric_limits<real>::infinity();
	
	for(Triangle *triag: *leaf->triags){
		IntersectionCompound tmp = triag->getIntersection(r);
		//test if there is a real hit AND if the hit triangle is nearer than the nearest so far
		if( tmp.t > 0 and tmp.t < nearest){
			inter = tmp;
			nearest = tmp.t;
		}
	}
	
	//if intersection => return inter.
	if (nearest < std::numeric_limits<real>::infinity())
		return;

	//if no intersection => extend oring to next border, add EPS 
	//boxMin/boxMax = position + dist * ray.origin 	
	real distToBox = r.intersectBox(boxMin, boxMax, position);
	
	if(distToBox < -eps){
		inter.t  = -1;
		return; 
	}
	
	//std::cout<<"old pos: "<<position <<std::endl;
	Vector3 oldPos = position;
	position +=  (distToBox+eps) * r.getDirection();//edd eps to be sureto be inside a box
	//std::cout<<"new pos: "<<position <<std::endl;

	//no next border => end(no intersection)
	if(    position[0] >= _boxMax[0] or position[1] >= _boxMax[1] or position[2] >= _boxMax[2]
	    or position[0] <= _boxMin[0] or position[1] <= _boxMin[1] or position[2] <= _boxMin[2]){
		inter.t  = -1;//no intersection, set dist to -1
		return;	
	}
	//TODO remove this after debugging	
	
	/*
	std::cout<<"warning: this should not happen -> one move should be outside the box"<<std::endl;
	std::cout<<"old pos: "<<oldPos <<std::endl;
	std::cout<<"new pos: "<<position <<std::endl;
	std::cout<<"dir: "<<r.getDirection()<<std::endl;
	std::cout<<"disToBox: "<<distToBox<<std::endl;
	std::cout<<"box min: "<<boxMin<<" max: "<<boxMax <<std::endl;
	r.intersectBox(boxMin, boxMax, oldPos,true);
	//assert(false);
	//recursive call of this function
	std::cout<<"was at: "<<oldPos <<"\tnow at: "<<position<<std::endl;
	*/
	//TODO: replace this with a while(true) loop
	_getIntersection(position, r, inter);
}		


IntersectionCompound KDTree::getIntersection(const Ray& r) const{
	IntersectionCompound inter;

	Vector3 position = r.getOrigin();
	//move position inside the cube!
	//if not possible: return
	
	if(    _boxMin[0]  > position[0] or position[0] > _boxMax[0]
            or _boxMin[1] > position[1] or position[1] > _boxMax[2]
	    or _boxMin[2] > position[2] or position[2] > _boxMax[2]){

		//std::cout<<"initial position "<<position<<" not in box min: "<<_boxMin<<" max:" << _boxMax <<std::endl;
		
		real distToBox = r.intersectBox(_boxMin, _boxMax);
		if(distToBox<0){//no intersection with the kdtree
			//std::cout<<"distToBox: "<< distToBox
			//<<" ,missed the kd-tree => abort" <<std::endl;
			//std::cout<<"starting at: "<<position<<"  direction: "<<r.getDirection()<<std::endl;		

			/*
			real t =  (_boxMin[2]-position[2])/r.getDirection()[2];
			auto newpos = (t+eps)*r.getDirection()+position;
			std::cout<<"t: "<<t<<" pos+ t*dir: "<< newpos<<std::endl;
			if(    _boxMin[0]  > newpos[0] or newpos[0] > _boxMax[0]
            or _boxMin[1] > newpos[1] or newpos[1] > _boxMax[2]
	    or _boxMin[2] > newpos[2] or newpos[2] > _boxMax[2])
				std::cout<<"inside box "<<std::endl;
			assert(false);
			*/
			return inter;
		}

		position += (distToBox+eps)*r.getDirection();
		//std::cout<<"moved to: "<<position <<std::endl;
	}
	//find the nearest intersection
	//std::cout<<"finding real intersection" <<std::endl;
	
	_getIntersection(position, r, inter);
	return 	inter;
}


