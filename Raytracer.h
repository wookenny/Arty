#pragma once

#include <vector>
#include <string>
#include <boost/unordered_map.hpp>

#include "common.h"

#include "Ray.h"
#include "Image.h"
#include "Lightsource.h"
#include "Color.h"
#include "Texture.h"
#include "Scene.h"

struct PrimaryRayBundle{
	Ray r;
	int x;
	int y;
	int sampling;
	Color color;	
};

class Raytracer{
	friend class TracingFunctor;

	private:
		Scene _scene;
		bool _debug;
		//methods used by the raytracer
		std::vector<PrimaryRayBundle> generatePrimaryRays() const;
		std::vector<PrimaryRayBundle> generatePrimaryRays(int x, int x_stepx, int y, int y_step) const;
		Color traceRay(const Ray&) const;
		void traceRays(std::vector<PrimaryRayBundle>&);
		std::vector<PrimaryRayBundle> generateAliasedRays(bool debug = false) const;
		Color localColor(const IntersectionCompound&,const Ray&) const;
		//TODO: anteil vom schatten zurückgeben
		bool inShadow(const Vector3& coord, const Vector3& light) const;		


	public:

		//Constructor
		Raytracer():_debug(false){ /*init();*/ }		

		//important methods
		void trace();
		void saveImage(const std::string& filename){ _scene.getImage().save(filename); }// method possibly changes the colors//change this
		
		void loadDefaultScene(){_scene.loadDefaultScene();}
		void loadScene(const std::string &xmlfile){_scene.loadScene(xmlfile);}		
};

struct TracingFunctor{

	Raytracer _rt;
	bool _showEdges;
	
	TracingFunctor(const Raytracer& rt,bool edges = false):_rt(rt),_showEdges(edges){
	
	}

	inline void operator()(std::vector<PrimaryRayBundle>::iterator a,
					  std::vector<PrimaryRayBundle>::iterator b) {
		std::vector<PrimaryRayBundle>::iterator iter = a;
		while(iter != b){	
			operator()(*iter);
			++iter;		
		}	
	}
	

	inline void operator() ( PrimaryRayBundle &raybundle) {
		Color &col =  raybundle.color;
		col = Color(0,0,0);
		//trace all rays and average the resulting color
		int s = raybundle.sampling; 
		for(int n=0; n<s; ++n)
			for(int m=0; m<s; ++m){
				//calculate right ray;
				Ray &ray = raybundle.r;
				//std::cout<< real(n)/s <<std::endl;
				//std::cout<< real(m)/s <<std::endl;

				real rightShift = _rt._scene.getWidth()*(raybundle.x+( (n%2==0?.5:-.5)*real(n)/s))/_rt._scene.getImage().getWidth();
				real downShift = _rt._scene.getHeight()*(raybundle.y+( (m%2==0?.5:-.5)*real(m)/s))/_rt._scene.getImage().getHeight();
				Vector3 &dir = ray.Direction();
				dir = _rt._scene.getUpperLeft() 
							+ _rt._scene.getRight() * rightShift  
							+ _rt._scene.getDown() * downShift
							- _rt._scene.getEye();
				dir.normalize();
				col += _rt.traceRay(ray);
				}
				
		col/=(s*s);
		//highlight edges
		if(s>1&&_showEdges)
			col = Color(1,0,0);
	}
};
