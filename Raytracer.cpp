#include "Raytracer.h"


#include <ctime>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <thread>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>


//fundamental methods:
std::vector<PrimaryRayBundle> Raytracer::generatePrimaryRays() const{
	std::vector<PrimaryRayBundle> primRays;
	const Image& img = _scene.getImage();
	for(unsigned int i= 0; i< img.getWidth(); ++i)
		 	for(unsigned int j= 0; j< img.getHeight(); ++j){
		 		PrimaryRayBundle pr;
		 		pr.x=i; pr.y=j;
		 		Ray ray;
		 		ray.setRaydepth(_scene.getMaxRayDepth());
		 		ray.setOrigin(_scene.getEye());
		 		Vector3 dir =  _scene.getUpperLeftCorner()
						+ (_scene.getRight() * (_scene.getWidth()*i/img.getWidth()) )
						+ (_scene.getDown() * (_scene.getHeight()*j/img.getHeight()) )
						- _scene.getEye();
				dir.normalize();
				ray.setDirection( dir );
		 		pr.r = ray;
		 		pr.sampling = 1;
		 		primRays.push_back(pr);
		 	}
	return primRays;
}


std::vector<PrimaryRayBundle> Raytracer::generatePrimaryRays(int x, int x_step, int y, int y_step) const{
	std::vector<PrimaryRayBundle>  primRays;
	const Image& img = _scene.getImage();

	for(int i = x;	i< std::min(static_cast<int>(img.getWidth()),(x+1)*x_step);++i)
		 for(unsigned int j= y; std::min(static_cast<int>(img.getWidth()),(y+1)*y_step); ++j){
		 		PrimaryRayBundle pr;
		 		pr.x=i; pr.y=j;
		 		Ray ray;
		 		ray.setRaydepth(_scene.getMaxRayDepth());
		 		ray.setOrigin(_scene.getEye());
		 		Vector3 dir = _scene.getUpperLeftCorner()
						+ (_scene.getRight() * (_scene.getWidth()*i/img.getWidth()) )
						+ (_scene.getDown() * (_scene.getHeight()*j/img.getHeight()) )
						- _scene.getEye();
				dir.normalize();
				ray.setDirection( dir );
		 		pr.r = ray;
		 		pr.sampling = 1;
		 		primRays.push_back(pr);
		 	}
	return primRays;
}




//replace by recursive ray calculation and color calculation, has to return a color
Color Raytracer::traceRay(const Ray& ray) const{
	Color col = Color(0,0,0);

	//intersection Informations
	IntersectionCompound intersection;
	intersection.t = -1;
	intersection.mat = 0;

	//test for nearest intersections
	for(unsigned int i=0; i< _scene.getNumObjects(); ++i){
		IntersectionCompound inter = _scene.getObject(i)->getIntersection(ray);
		if(inter.t>0 && ( inter.t < intersection.t or intersection.t==-1))
			intersection = inter;
	}

	//calculate color and recursive call
	if( intersection.t > eps and intersection.mat!=0){//something hit?
		col = localColor(intersection,ray);

		if(intersection.mat->getReflection()>eps and ray.getDepth()>0){
			Ray reflectedRay = Ray::getReflectedRay(ray,intersection.t,intersection.normal);
			col += intersection.mat->getReflection()*traceRay(reflectedRay);
		//TODO: tranparenz erweitern
		}
	}

	return col;
}


void Raytracer::traceRays(std::vector<PrimaryRayBundle>& rays){


	
	const Raytracer &rt = *this;
	TracingFunctor t_func( rt );

	for(uint i=0; i<rays.size();++i)
		t_func(rays.at(i));

}


Color Raytracer::localColor(const IntersectionCompound& inter,const Ray& ray) const{//TODO: init scene here
	Material &m = *inter.mat;
	Vector3 hitpoint = ray.getOrigin()+inter.t * ray.getDirection();
	Color local_color = m.getColor(inter);
	Color c =  _scene.getAmbient()*local_color *m.getAmbient(); //ambient term
	//diffuse term
	for(unsigned int i=0; i<_scene.getNumLights() ; ++i){
		const Vector3 &pos = _scene.getLight(i).getPosition();
		if( inShadow(hitpoint,pos) )
			continue;
		Vector3 lightdir = pos - hitpoint;
		lightdir.normalize();
		//TODO get shadow percentage
		c += _scene.getLight(i).getIntensity()*_scene.getLight(i).getColor()*m.getDiffuse()*local_color
				*std::max(real(0),inter.normal.dot(lightdir));
	}

	//specular term
	for(unsigned int i=0; i<_scene.getNumLights() ; ++i){
		const Vector3 &pos = _scene.getLight(i).getPosition();
		if( inShadow(hitpoint,pos) )
			continue;
		Vector3 lightdir = pos - hitpoint;
		lightdir.normalize();
		Vector3 toEye = _scene.getEye() - hitpoint;
		toEye.normalize();
		Vector3 h  = lightdir + toEye;
		h.normalize();
		//TODO get shadow percentage and nearest lightpoint
		c += _scene.getLight(i).getIntensity()*_scene.getLight(i).getColor()*m.getSpecular()
			* std::pow(std::max(real(0),inter.normal.dot(h)),m.getShininess());
	}


	return c;
}


std::vector<PrimaryRayBundle> Raytracer::generateAliasedRays(bool debug) const{
	//find pixel to trace
	const Image &img = _scene.getImage();
	std::vector<std::vector<bool> > edges = img.findEdges();
	std::vector<PrimaryRayBundle> aliasedRays;
	for(unsigned int i= 0; i< edges.size(); ++i)
		 	for(unsigned int j= 0; j< edges[i].size(); ++j)
		 		if(edges[i][j]){
				 		PrimaryRayBundle pr;
				 		pr.x=i; pr.y=j;
						if(debug){
							pr.color = Color(1,0,0);
							continue;
						}
				 		Ray ray;
				 		ray.setRaydepth(_scene.getMaxRayDepth());
				 		ray.setOrigin(_scene.getEye());
				 		Vector3 dir = _scene.getUpperLeftCorner()
								+ (_scene.getRight()
								* (_scene.getWidth()*i/_scene.getImage().getWidth()) )
								+ (_scene.getDown()
								* (_scene.getHeight()*j/_scene.getImage().getHeight()) )
								- _scene.getEye();
						dir.normalize();
						ray.setDirection( dir );
				 		pr.r = ray;
				 		pr.sampling = _scene.getSampling();
				 		aliasedRays.push_back(pr);
		 			}
	return aliasedRays;
}


void Raytracer::trace(){
	time_t start,stop;

	time(&start);
	std::vector<PrimaryRayBundle> primRays = generatePrimaryRays();//generate rays
	traceRays(primRays);

	//copy all the color into the image
	Image &img = _scene.getImage();
	for(unsigned int i = 0; i<primRays.size(); ++i)
		img.at(primRays[i].x,primRays[i].y) = primRays[i].color;

	//remove uogly edges
	std::vector<PrimaryRayBundle> aliasRays;
	if(_scene.getSampling()>0)
		aliasRays = generateAliasedRays(_debug);

	if(not _debug)
		traceRays( aliasRays );

	//copy all the color into the image
	for(unsigned int i = 0; i<aliasRays.size(); ++i)
		img.at(aliasRays[i].x,aliasRays[i].y) = aliasRays[i].color;


	time(&stop);
	std::cout<<"Runtime: "<<difftime(stop,start)<<" seconds."<<std::endl;
}


bool Raytracer::inShadow(const Vector3& coord, const Vector3& light) const{
	Vector3 lightdir = light - coord;
	real distance = lightdir.length();
	lightdir.normalize();
	Ray ray(coord+eps*lightdir,lightdir);

	for(unsigned int i=0; i<_scene.getNumObjects(); ++i){
		IntersectionCompound inter;
		//prevent jumps on uninitialised values
		inter.t = -1;

		inter = _scene.getObject(i)->getIntersection(ray);
		if(inter.t > eps )
			if(inter.t < distance)
				return true;

	}
	return false;
}

