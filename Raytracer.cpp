#include "Raytracer.h"
#include "pugixml.hpp"
#include "pugiconfig.hpp"

#include <ctime>
#include <algorithm>
//#include <parallel/algorithm>
/** compilieren mit -fopenmp **/
#include <cmath>
#include <iostream>
#include <omp.h>

//use this function to initialize raytracer each constructor
void Raytracer::loadDefaultScene(){

		_eye = Vector3(eps,eps,eps-2);//not (0,0,0) because of rounding errors
 		_right = Vector3(1,0,0);
 		_down = Vector3(0,-1,0);
		_upperLeftCorner = Vector3(-1.,1.,1);
		_width = 2.; //size of viewing window
		_height = 2.;
		_pixelPerUnit = 400;
		_superSampling = 4;
		_tracedImage = Image(_width*_pixelPerUnit, _height*_pixelPerUnit);
		_tracedImage.setGamma(2.2);
		_maxRayDepth = 4;	
		_ambient = 0.05*Color(1,1,1);
		
		//KD-Tree objects
		Material mat1,mat2,mat3,mat4;
		mat1.setColor(Color(1,0,0));
		mat2.setColor(Color(0,1,0));		
		mat3.setColor(Color(1,1,1));	
		mat4.setColor(Color(0.0,0.0,0.3));
		mat4.setSpecular(0.2);	
		mat4.setShininess(64);
		_materials["mat1"] = mat1;_materials["mat2"] = mat2;
		_materials["mat3"] = mat3;_materials["mat4"] = mat4;
	
		_vertices["LUH"] = Vector3(-1,-1,3);
		_vertices["RUH"] = Vector3(1,-1,3);
		_vertices["ROH"] = Vector3(1,1,3);
		_vertices["LOH"] = Vector3(-1,1,3);
		_vertices["LUV"] = Vector3(-1,-1,1);
		_vertices["RUV"] = Vector3(1,-1,1);
		_vertices["ROV"] = Vector3(1,1,1);
		_vertices["LOV"] = Vector3(-1,1,1);
		
		
		//hinten
		Triangle* t = new Triangle(_vertices["LUH"],_vertices["ROH"],_vertices["RUH"]);
		t->setMaterial(&_materials["mat3"]);
		_objects.push_back(t);//pointers because of polymorphism
		
		t = new Triangle(_vertices["LUH"],_vertices["LOH"],_vertices["ROH"]);
		t->setMaterial(&_materials["mat3"]);
		_objects.push_back(t);//pointers because of polymorphism

		//links
		t = new Triangle(_vertices["LUV"],_vertices["LOH"],_vertices["LUH"]);
		t->setMaterial(&_materials["mat1"]);
		_objects.push_back(t);//pointers because of polymorphism
		t = new Triangle(_vertices["LUV"],_vertices["LOV"],_vertices["LOH"]);
		t->setMaterial(&_materials["mat1"]);
		_objects.push_back(t);//pointers because of polymorphism
		//rechts
		t = new Triangle(_vertices["RUV"],_vertices["RUH"],_vertices["ROH"]);
		t->setMaterial(&_materials["mat2"]);
		_objects.push_back(t);//pointers because of polymorphism
		t = new Triangle(_vertices["RUV"],_vertices["ROH"],_vertices["ROV"]);
		t->setMaterial(&_materials["mat2"]);
		_objects.push_back(t);//pointers because of polymorphism

		//unten
		t = new Triangle(_vertices["LUV"],_vertices["LUH"],_vertices["RUH"]);
		t->setMaterial(&_materials["mat3"]);
		_objects.push_back(t);//pointers because of polymorphism
		t = new Triangle(_vertices["LUV"],_vertices["RUH"],_vertices["RUV"]);
		t->setMaterial(&_materials["mat3"]);
		_objects.push_back(t);//pointers because of polymorphism


				//new sphere
		Sphere* s = new Sphere(Vector3(.3,-1+.3,2),0.3);
		s->setMaterial(&_materials["mat4"]);
		_objects.push_back(s);//pointers because of polymorphism


		Vector3 light(0.,1,2);
		//s = new Sphere(light,0.1);
		//s->setMaterial(mat3);
		//_objects.push_back(s);//pointers because of polymorphism
		_lights.push_back(Lightsource(light, 0.5, Color(1,1,1)));
		

}


void Raytracer::loadScene(const std::string &xmlfile){
	std::cout<<"loading scene "<<xmlfile<<std::endl;
	//code_load_file
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(xmlfile.c_str());
 
        //getting general settings:
        pugi::xml_node child = doc.child("Scene").child("Boundaries");
	for (pugi::xml_node_iterator it = child.begin(); it != child.end(); ++it){
		pugi::xml_attribute_iterator ait = it->attributes_begin();
		std::string name = ait->value();
		if ( name == std::string("Eye") ){
			++ait;
			_eye = Vector3(ait->value());
			std::cout<<"Eye = "<<_eye<<std::endl;
 		}
		else if ( name == std::string("Right") ){
			++ait;
			_right = Vector3(ait->value());
			std::cout<<"Right = "<<_right<<std::endl;
 		}
		else if ( name == std::string("Down") ){
			++ait;
			_down = Vector3(ait->value());
			std::cout<<"Down = "<<_down<<std::endl;
 		}
		else if ( name == std::string("UpperLeftCorner") ){
			++ait;
			_upperLeftCorner = Vector3(ait->value());
			std::cout<<"UpperLeftCorner = "<<_upperLeftCorner<<std::endl;
 		}
		else if ( name == std::string("Width") ){
			++ait;
			_width = ait->as_float();
			std::cout<<"Width = "<<_width<<std::endl;
		}
		else if ( name == std::string("Height") ){
			++ait;
			_height = ait->as_float();
			std::cout<<"Height = "<<_height<<std::endl;
		}
		else if ( name == std::string("PixelPerUnit") ){
			++ait;
			_pixelPerUnit = ait->as_int();
			std::cout<<"PixelPerUnit = "<<_pixelPerUnit<<std::endl;
		}
		else if ( name == std::string("Supersampling") ){
			++ait;
			_superSampling = ait->as_int();
			std::cout<<"Supersampling = "<<_superSampling<<std::endl;
		}
		//additional settings:
		_maxRayDepth = 4;	
		_tracedImage = Image(_width*_pixelPerUnit, _height*_pixelPerUnit);
		_tracedImage.setGamma(2.2);
	}
	 

	//getting light settings:
        child = doc.child("Scene").child("Lights");
	for (pugi::xml_node_iterator it = child.begin(); it != child.end(); ++it){
		std::string type = it->attribute("Type").value();		
		if(type == "ambient"){
			_ambient = 0.05*Color(it->attribute("Color").value());
			std::cout<<"ambient = "<<_ambient<<std::endl;		
		}else if( type == "point"){
			Vector3 lightpos(it->attribute("Position").value());
			_lights.push_back(Lightsource(lightpos, it->attribute("Intensity").as_float(), 
					Color(it->attribute("Color").value())));

		}else{
			std::cout<<"Warning: light of type "<<type<<" not supported!"<<std::endl;		
		}

	}

	//getting materials:
        child = doc.child("Scene").child("Materials");
	for (pugi::xml_node_iterator it = child.begin(); it != child.end(); ++it){
		std::string name = it->attribute("Name").value();		
		Material tmpmat;
		tmpmat.setColor(Color(it->attribute("Color").value()));
		if( it->attribute("Specular") )
			tmpmat.setSpecular(it->attribute("Specular").as_float());
		if( it->attribute("Shininess") )
			tmpmat.setShininess(it->attribute("Shininess").as_int());
		_materials[name] = tmpmat;
		std::cout<<"Added material "<<name<<std::endl;
	}
	
	//get implicit objects 
	child = doc.child("Scene").child("Geometry").child("Implicits");
	for (pugi::xml_node_iterator it = child.begin(); it != child.end(); ++it){
		std::string type = it->attribute("Type").value();
		if(type == "Sphere"){	
			Sphere* s = new Sphere( Vector3(it->attribute("Position").value()),
						it->attribute("Radius").as_float());
			
			s->setMaterial(&_materials[it->attribute("Material").value()]);
			_objects.push_back(s);//pointers because of polymorphism
			std::cout<<"Added object of type "<<type<<std::endl;
		}else{
			std::cout<<"Warning: implicit object with type "<<type<<" not supported!"<<std::endl;		
		}
	}

	//get vertices 
	child = doc.child("Scene").child("Geometry").child("Points");
	for (pugi::xml_node_iterator it = child.begin(); it != child.end(); ++it){
		std::string name = it->attribute("Name").value();
		_vertices[name] = Vector3(  it->attribute("Position").value() );
		std::cout<<"Added point "<<name<<std::endl;
	}

	//get triangles
	child = doc.child("Scene").child("Geometry").child("Triangles");
	for (pugi::xml_node_iterator it = child.begin(); it != child.end(); ++it){
		std::string name = it->attribute("Name").value();

		Triangle *t = new Triangle( 	_vertices[it->attribute("A").value()],
						_vertices[it->attribute("B").value()],
						_vertices[it->attribute("C").value()]);
		t->setMaterial(&_materials[ it->attribute("Material").value() ]);
		_objects.push_back(t);
		std::cout<<"Added triangle "<<name<<std::endl;
	}
}


//fundamental methods:
std::vector<PrimaryRayBundle> Raytracer::generatePrimaryRays() const{
	std::vector<PrimaryRayBundle> primRays;
	
	for(unsigned int i= 0; i< _tracedImage.getWidth(); ++i)
		 	for(unsigned int j= 0; j< _tracedImage.getHeight(); ++j){
		 		PrimaryRayBundle pr;
		 		pr.x=i; pr.y=j;
		 		Ray ray;
		 		ray.setRaydepth(_maxRayDepth);
		 		ray.setOrigin(_eye);
		 		Vector3 dir = _upperLeftCorner 
						+ (_right * (_width*i/_tracedImage.getWidth()) ) 
						+ (_down * (_height*j/_tracedImage.getHeight()) )
						- _eye;
				dir.normalize();
				ray.setDirection( dir );				
		 		pr.r = ray;
		 		pr.sampling = 1;
		 		primRays.push_back(pr);
		 	}
	return primRays;
}


//replace by recursive ray calculation and color calculation, has to return a color
Color Raytracer::traceRay(const Ray& ray){
	Color col = Color(0,0,0);
	
	//intersection Informations
	IntersectionCompound intersection;
	intersection.t = -1;
	intersection.mat = 0;

	//test for nearest intersections
	for(unsigned int i=0; i<_objects.size(); ++i){
		IntersectionCompound inter =_objects.at(i)->getIntersection(ray);
		if(inter.t>0 && ( inter.t < intersection.t or intersection.t==-1))
			intersection = inter;
	}
	
	//calculate color and recursive call
	if( intersection.t > eps and intersection.mat!=0){//something hit?
		col = localColor(intersection,ray);	

		if(intersection.mat->getSpecular()>eps || ray.getDepth()>0){	
			Ray reflectedRay = Ray::getReflectedRay(ray,intersection.t,intersection.normal);
			col += intersection.mat->getSpecular()*traceRay(reflectedRay);		
		//TODO: tranparenz erweitern
		}
	}
		
	return col;
}


void Raytracer::traceRays(const std::vector<PrimaryRayBundle>& rays){
	int num_threads = 2*omp_get_max_threads();

	TracingFunctor t_func( *this );
	std::cout<< rays.size()<<" rays to trace"<<std::endl;
/*
	//build blocks
	typedef std::vector<PrimaryRayBundle>::const_iterator RayIter;
	std::vector< std::pair<RayIter,RayIter> > rayblocks;
	int stepsize = rays.size()/num_threads;
	int count = 0;
	RayIter a=rays.begin();
	while(count+stepsize<rays.size()){
		rayblocks.push_back( std::make_pair(a,a+stepsize) );
		a += stepsize;
		count += stepsize;	
	}
	if(a!=rays.end()){
		std::pair<RayIter,RayIter> rpair = 	std::make_pair(a,rays.end());
		rayblocks.push_back(rpair );
	}
	std::cout<<"build "<<rayblocks.size()<<" blocks"<<std::endl;
	
	//#pragma omp parallel for
	for(unsigned int i=0; i<rayblocks.size();++i)
		t_func(rayblocks.at(i).first,rayblocks.at(i).second);
	*/
	
	#pragma omp parallel for
	for(unsigned int i=0; i<rays.size();++i)
		t_func(rays.at(i));
	
	
}


Color Raytracer::localColor(const IntersectionCompound& inter,const Ray& ray) const{
	Material &m = *inter.mat;
	Vector3 hitpoint = ray.getOrigin()+inter.t * ray.getDirection();
	Color local_color = m.getColor(inter.px,inter.py);
	Color c =  _ambient*local_color *m.getAmbient(); //ambient term
	//diffuse term
	for(unsigned int i=0; i<_lights.size() ; ++i){
		if( inShadow(hitpoint,_lights.at(i).getPosition()))
			continue;
		Vector3 lightdir = _lights.at(i).getPosition() - hitpoint;
		lightdir.normalize();
		//TODO get shadow percentage
		c += _lights.at(i).getIntensity()*_lights.at(i).getColor()*m.getDiffuse()*local_color 
				*std::max(real(0),inter.normal.dot(lightdir));
	}
	
	//specular term
	for(unsigned int i=0; i<_lights.size() ; ++i){
		if( inShadow(hitpoint,_lights.at(i).getPosition()))
			continue;
		Vector3 lightdir = _lights.at(i).getPosition() - hitpoint;
		lightdir.normalize();
		Vector3 toEye = _eye - hitpoint;
		toEye.normalize();
		Vector3 h  = lightdir + toEye;
		h.normalize();
		//TODO get shadow percentage and nearest lightpoint
		c += _lights.at(i).getIntensity()*_lights.at(i).getColor()*m.getSpecular()
			* std::pow(std::max(real(0),inter.normal.dot(h)),m.getShininess());
	}
	
	
	return c;
}		


void Raytracer::antialiase(bool debug){
	//find pixel to trace 	
	std::vector<std::vector<bool> > edges = _tracedImage.findEdges();
	std::vector<PrimaryRayBundle> aliasedRays;	
	for(unsigned int i= 0; i< edges.size(); ++i)
		 	for(unsigned int j= 0; j< edges.at(i).size(); ++j)
		 		if(edges.at(i).at(j)){
		 			if(debug){
		 				_tracedImage.at(i,j) = Color(1,0,0);
		 			}else{
				 		PrimaryRayBundle pr;
				 		pr.x=i; pr.y=j;
				 		Ray ray;
				 		ray.setRaydepth(_maxRayDepth);
				 		ray.setOrigin(_eye);
				 		Vector3 dir = _upperLeftCorner 
								+ (_right * (_width*i/_tracedImage.getWidth()) ) 
								+ (_down * (_height*j/_tracedImage.getHeight()) )
								- _eye;
						dir.normalize();
						ray.setDirection( dir );				
				 		pr.r = ray;
				 		pr.sampling = _superSampling;
				 		aliasedRays.push_back(pr);
		 			}
		 		}
	//trace pixels
	//TODO: possibility to save one sampling step, because color is in _tracedImage
	if(debug)
		return;
	std::cout	<<"smooth on "<<100*aliasedRays.size()/(_width*_pixelPerUnit*_height*_pixelPerUnit)
			<<"% of the pixel" <<std::endl;
	traceRays( aliasedRays );
}


void Raytracer::trace(){
	time_t start,stop;
	
	time(&start);
	std::vector<PrimaryRayBundle> primRays = generatePrimaryRays();//generate rays
	traceRays(primRays);
	std::cout<<"start antialiasing"<<std::endl;
	antialiase(); //remove ugly egdes
	time(&stop);
	std::cout<<"Runtime: "<<difftime(stop,start)<<" seconds."<<std::endl;
}

bool Raytracer::inShadow(const Vector3& coord, const Vector3& light) const{
	Vector3 lightdir = light - coord;
	real distance = lightdir.length();
	lightdir.normalize();
	Ray ray(coord+eps*lightdir,lightdir);
	
	for(unsigned int i=0; i<_objects.size(); ++i){
		IntersectionCompound inter;
		//prevent jumps on uninitialised values
		inter.t = -1; 

		inter =_objects.at(i)->getIntersection(ray);
		if(inter.t > eps )
			if(inter.t < distance)
				return true;
		
	}
	return false;
}		

