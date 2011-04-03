#include "Raytracer.h"
#include "pugixml.hpp"
#include "pugiconfig.hpp"

#include <ctime>
#include <algorithm>
//#include <parallel/algorithm>
/** compilieren mit -fopenmp **/
#include <cmath>
#include <iostream>

//use this function to initialize raytracer each constructor
void Raytracer::loadDefaultScene(){

		_eye = Vector3(eps,eps,eps-2);//not (0,0,0) because of rounding errors
 		_right = Vector3(1,0,0);
 		_down = Vector3(0,-1,0);
		_upperLeftCorner = Vector3(-1.,1.,1);
		_width = 2.; //size of viewing window
		_height = 2.;
		_pixelPerUnit = 400;
		_superSampling = 10;
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
	
		Vector3 LUH(-1,-1,3);
		Vector3 RUH(1,-1,3);
		Vector3 ROH(1,1,3);
		Vector3 LOH(-1,1,3);
		Vector3 LUV(-1,-1,1);
		Vector3 RUV(1,-1,1);
		Vector3 ROV(1,1,1);
		Vector3 LOV(-1,1,1);
		
		
		//hinten
		Triangle* t = new Triangle(LUH,ROH,RUH);
		t->setMaterial(&_materials["mat3"]);
		_objects.push_back(t);//pointers because of polymorphism
		
		t = new Triangle(LUH,LOH,ROH);
		t->setMaterial(&_materials["mat3"]);
		_objects.push_back(t);//pointers because of polymorphism

		//links
		t = new Triangle(LUV,LOH,LUH);
		t->setMaterial(&_materials["mat1"]);
		_objects.push_back(t);//pointers because of polymorphism
		t = new Triangle(LUV,LOV,LOH);
		t->setMaterial(&_materials["mat1"]);
		_objects.push_back(t);//pointers because of polymorphism
		//rechts
		t = new Triangle(RUV,RUH,ROH);
		t->setMaterial(&_materials["mat2"]);
		_objects.push_back(t);//pointers because of polymorphism
		t = new Triangle(RUV,ROH,ROV);
		t->setMaterial(&_materials["mat2"]);
		_objects.push_back(t);//pointers because of polymorphism

		//unten
		t = new Triangle(LUV,LUH,RUH);
		t->setMaterial(&_materials["mat3"]);
		_objects.push_back(t);//pointers because of polymorphism
		t = new Triangle(LUV,RUH,RUV);
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
		else if ( name == std::string("SuperSampling") ){
			++ait;
			_superSampling = ait->as_int();
			std::cout<<"SuperSampling = "<<_superSampling<<std::endl;
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
		//TODO: um reflektion und tranparenz erweitern
		}
	}
		
	return col;
}


void Raytracer::traceRays(const std::vector<PrimaryRayBundle>& rays){


	TracingFunctor t_func( *this );
	std::cout<< rays.size()<<" rays to trace"<<std::endl;
	std::for_each(rays.begin(),rays.end(),t_func);	//serial version in <algorithm>	
	//__gnu_parallel::for_each(rays.begin(),rays.end(),t_func); //parallel version in	<parallel/algorithm>
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

