#pragma once

#include <vector>
#include <string>
# include <boost/unordered_map.hpp>

#include "common.h"
#include "Obstacle.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Ray.h"
#include "Image.h"
#include "Lightsource.h"
#include "Color.h"

struct PrimaryRayBundle{
	Ray r;
	int x;
	int y;
	int sampling;	
};

class Raytracer{
	friend class TracingFunctor;

	private:
		Vector3 _eye, _down, _right, _upperLeftCorner;
		real _width, _height;//size of viewing window
		real _pixelPerUnit;
		Image _tracedImage;
		int _maxRayDepth;
		int _superSampling; //calculate s x s Rays for each pixel on an edge 	
		//KD-Tree objects
		std::vector<Obstacle*> _objects;//pointers because of polymorphism
		std::vector<Lightsource> _lights;
		boost::unordered_map<std::string, Vector3> _vertices;
		boost::unordered_map<std::string, Material> _materials;

		//gamma	?
		
		//ambient lightning	
		Color _ambient;

		//methods used by the raytracer
		std::vector<PrimaryRayBundle> generatePrimaryRays() const;
		Color traceRay(const Ray&);
		void traceRays(const std::vector<PrimaryRayBundle>&);
		void antialiase(bool debug = false);
		Color localColor(const IntersectionCompound&,const Ray&) const;
		//TODO: anteil vom schatten zurückgeben
		bool inShadow(const Vector3& coord, const Vector3& light) const;		

	public:

		//Constructor
		Raytracer(){ /*init();*/ }		
		//setter
		void setEyepoint(const Vector3& e){_eye = e;}
		void setDown(const Vector3& d){_down = d;}		
		void setRight(const Vector3& r){_right = r;}
		void setUpperLeftCorner(const Vector3& c){_upperLeftCorner = c;}
		void setWidth(real w){ _width = w; }
		void setHeight(real h){ _height = h; }
		void setPixelPerUnit(const real p){ _pixelPerUnit = p;}	
		void setImage(const Image& img){ _tracedImage = img; }
		void setMaxRayDepth(const int d){ _maxRayDepth = d;}
		void setObjects(const std::vector<Obstacle*>& o){ _objects =o;}
		void setLightsources(const std::vector<Lightsource>& l){ _lights = l; }
		
		void loadScene(const std::string &xmlfile);	
		void loadDefaultScene();
		//important methods
		void trace();
		void saveImage(const std::string& filename){ _tracedImage.save(filename); }// method possibly changes the colors//change this
		
			
};

struct TracingFunctor{

	Raytracer& _rt;
	bool _showEdges;
	
	TracingFunctor(Raytracer& rt,bool edges = false):_rt(rt),_showEdges(edges){
	
	}

	void operator()(const std::vector<PrimaryRayBundle>::const_iterator a,
					 const std::vector<PrimaryRayBundle>::const_iterator b) {
		std::vector<PrimaryRayBundle>::const_iterator iter = a;
		while(iter != b){	
			operator()(*iter);
			++iter;		
		}	
	}

	void operator() (const PrimaryRayBundle &raybundle) {
		Color col = Color(0,0,0);
		//trace all rays and average the resulting color
		int s = raybundle.sampling; 
		for(int n=0; n<s; ++n)
			for(int m=0; m<s; ++m){
				//calculate right ray;
				Ray ray = raybundle.r;
				//std::cout<< real(n)/s <<std::endl;
				//std::cout<< real(m)/s <<std::endl;

				real rightShift = _rt._width*(raybundle.x+( (n%2==0?.5:-.5)*real(n)/s))/_rt._tracedImage.getWidth();
				real downShift = _rt._height*(raybundle.y+( (m%2==0?.5:-.5)*real(m)/s))/_rt._tracedImage.getHeight();
				Vector3 dir = _rt._upperLeftCorner 
							+ _rt._right * rightShift  
							+ _rt._down * downShift
							- _rt._eye;
				dir.normalize();
				ray.setDirection( dir );
				col += _rt.traceRay(ray);
				}
				
		_rt._tracedImage.at( raybundle.x, raybundle.y) = col/(s*s);
		//highlight edges
		if(s>1&&_showEdges)
			_rt._tracedImage.at( raybundle.x, raybundle.y) =Color(1,0,0);
		
	}
};
