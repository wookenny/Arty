#include "Raytracer.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <thread>
#include <fstream>
#include <tuple>
#include <chrono>
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


std::vector<PrimaryRayBundle> Raytracer::generatePrimaryRays(int x1, int x2, int y1, int y2) const{
	std::vector<PrimaryRayBundle>  primRays;
	const Image& img = _scene.getImage();

	for(int i = x1;	i< x2;++i)
		 for(int j= y1; j < y2; ++j){
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

	Raytracer *rt = this;
	TracingFunctor t_func( rt );

	for(uint i=0; i<rays.size();++i)
		t_func(rays[i]);

}


Color Raytracer::localColor(const IntersectionCompound& inter,const Ray& ray) const{//TODO: init scene here


	Material &m = *inter.mat;
	Vector3 hitpoint = ray.getOrigin()+inter.t * ray.getDirection();

	//get all shadow percentages ONCE:
	std::vector<real> shadows;
	int shadow_sampling = 7;//TODO: better position to set up light sampling param
	for(unsigned int i=0; i<_scene.getNumLights() ; ++i)
		shadows.push_back(inShadow(hitpoint,_scene.getLight(i),shadow_sampling));



	//ambient term
	Color local_color = m.getColor(inter);
	Color c =  _scene.getAmbientIntensity()*_scene.getAmbient()*local_color *m.getAmbient();


	//diffuse term
	for(unsigned int i=0; i<_scene.getNumLights() ; ++i){
		const Vector3 &pos = _scene.getLight(i).getPosition();
		if(shadows[i] > 1-eps)
			continue;
		Vector3 lightdir = pos - hitpoint;
		lightdir.normalize();
		//TODO get shadow percentage
		c += (1-shadows[i])*_scene.getLight(i).getIntensity()*
			_scene.getLight(i).getColor()*m.getDiffuse()*local_color
				*std::max(real(0),inter.normal.dot(lightdir));
	}

	//specular term
	for(unsigned int i=0; i<_scene.getNumLights() ; ++i){

		std::vector<Vector3> lightdirs;
		_scene.getLight(i).getDirectionsToLight( hitpoint, shadow_sampling, lightdirs);
		real max_spec = 0;
		foreach(Vector3 &lightdir, lightdirs){
			lightdir.normalize();
			Vector3 toEye = _scene.getEye() - hitpoint;
			toEye.normalize();
			Vector3 h  = lightdir + toEye;
			h.normalize();
			max_spec = std::max(max_spec,inter.normal.dot(h));
		}

		c += (1-shadows[i])*_scene.getLight(i).getIntensity()*_scene.getLight(i).getColor()*m.getSpecular()
			* std::pow(max_spec,m.getShininess());
	}


	return c;
}


std::vector<PrimaryRayBundle> Raytracer::generateAliasedRays(int x1, int x2, int y1, int y2,
						const std::vector<std::vector<bool> > *edges, bool debug) const{
	//find pixel to trace
	std::vector<PrimaryRayBundle> aliasedRays;
	for(int i= x1; i< x2; ++i)
		 	for(int j= y1; j< y2; ++j)
		 		if( (*edges)[i][j]){
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


//regiona based version
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

void Raytracer::trace(int t){
	if (t>0) num_cores = t;
	std::cout<<"starting tracing with "<< num_cores <<" threads"<<std::endl;
	//time stuff
	typedef std::chrono::high_resolution_clock Clock;
	typedef std::chrono::duration<double> sec;
	Clock::time_point start, stop;

	start = Clock::now();
	//create num core threads
	//and create enough chunks for them to work on
	//at the end: join
	std::vector<std::thread> threads;
	tilesToTrace_.clear();
	curr_ = 0;
	uint x = 0,y = 0, x_step = 10, y_step = 10;//TODO fix stepsize

	while( x <  _scene.getImage().getWidth()){
		uint x2 = std::min(x+x_step,_scene.getImage().getWidth());
		y = 0;
		while( y < _scene.getImage().getHeight() ){
			uint y2 = std::min(y+y_step,_scene.getImage().getHeight());
			tilesToTrace_.push_back(std::make_tuple(x,x2,y,y2));
			y = y2;
		}
		x = x2;
	}

	//create new thread here
	for(uint i =0; i<num_cores; ++i){
		threads.push_back(
			std::thread{TracingFunctor(this)}
		);
	}

	foreach(auto &t, threads)
		t.join();

	//TODO: delete old call:
	//std::vector<PrimaryRayBundle> primRays = generatePrimaryRays();//generate rays
	//traceRays(primRays);
	stop = Clock::now();
	auto time1 = sec(stop-start).count();
	std::cout<<"Runtime: "<<time1<<" seconds"<<std::endl;

	start = Clock::now();
	//remove ugly edges
	if( _scene.getSampling()>0){
		const std::vector<std::vector<bool> > edges = _scene.getImage().findEdges();
		curr_ = 0;
		threads.clear();
		for(uint i =0; i<num_cores; ++i){
		threads.push_back(
				std::thread{TracingFunctor(this,_debug,&edges)}
			);
		}
		foreach(auto &t, threads)
			t.join();
		//traceRays( aliasRays );
	}
	stop = Clock::now();
	auto time2 = sec(stop-start).count();
	std::cout<<"Runtime(antialias): "<<time2<<" seconds"<<std::endl;
	std::cout<<"Total running time: "<<time1+time2<<" seconds"<<std::endl;

}


real Raytracer::inShadow(const Vector3& coord, const Lightsource &light, int num_samples) const{
	real shadow_hits = 0;

	//Vector3 lightdir = light - coord;
	std::vector<Vector3> lightdirs;
	light.getDirectionsToLight( coord, num_samples, lightdirs);
	foreach(Vector3& lightdir, lightdirs){
		real distance = lightdir.length();
		lightdir.normalize();
		Ray ray(coord+eps*lightdir,lightdir);

		//TODO: do something better here
		for(unsigned int i=0; i<_scene.getNumObjects(); ++i){
			IntersectionCompound inter;
			//prevent jumps on uninitialised values
			inter.t = -1;

			inter = _scene.getObject(i)->getIntersection(ray);
			if(inter.t > eps  and inter.t < distance){
					shadow_hits+=1;
					break;	//no need to find out how much objects block the view
				}
		}
	}
	//1 => totally in shadow, 0 => no shadow at all
	return shadow_hits/lightdirs.size();
}

