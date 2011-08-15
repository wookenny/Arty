#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <thread>
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
		std::vector<PrimaryRayBundle> generatePrimaryRays(int x1, int x2, int y1, int y2) const;
		Color traceRay(const Ray&) const;
		void traceRays(std::vector<PrimaryRayBundle>&);
		std::vector<PrimaryRayBundle> generateAliasedRays(bool debug = false) const;
		std::vector<PrimaryRayBundle> generateAliasedRays(int x1, int x2, int y1, int y2,
						const std::vector<std::vector<bool> > *edges, bool debug = false) const;
		Color localColor(const IntersectionCompound&,const Ray&) const;
		real inShadow(const Vector3& coord, const Lightsource &light,int num_samples = 1) const;

		//multithreading parts
		uint num_cores;
		std::mutex m_;
		std::vector<std::tuple<int,int,int,int> > tilesToTrace_;
		uint curr_;

	public:

		//Constructor
		Raytracer():_debug(false),num_cores(sysconf( _SC_NPROCESSORS_ONLN )){ /*init();*/ }

		//important methods
		void trace(int t=0);
		void saveImage(const std::string& filename){ _scene.getImage().save(filename); }// method possibly changes the colors//change this

		void loadDefaultScene(){_scene.loadDefaultScene();}
		void loadScene(const std::string &xmlfile){_scene.loadScene(xmlfile);}
};

struct TracingFunctor{

	Raytracer *_rt;
	const std::vector<std::vector<bool> > *_edges;
	bool _showEdges;


	TracingFunctor(Raytracer *rt,bool show = false,
			const std::vector<std::vector<bool> > *edges = 0):_rt(rt),_edges(edges),_showEdges(show){}

	inline void operator()(){
		std::tuple<int,int,int,int> t;
		while( true ){
			{//scope for mutex
				std::lock_guard<std::mutex> lk(_rt->m_);
				if( _rt->curr_ >= _rt->tilesToTrace_.size() )
					break;
				t = _rt->tilesToTrace_[_rt->curr_];
				_rt->curr_++;
			}



			std::vector<PrimaryRayBundle> p = (_edges == 0)?
							_rt->generatePrimaryRays(std::get<0>(t),std::get<1>(t),std::get<2>(t),std::get<3>(t)):
							_rt->generateAliasedRays(std::get<0>(t),std::get<1>(t),std::get<2>(t),std::get<3>(t),
							_edges, _showEdges);
			//for(PrimaryRayBundle &b : p)
			//	operator()(b);
			foreach(PrimaryRayBundle &b, p)
				operator()(b);

		}

	}


	inline void operator() ( PrimaryRayBundle &raybundle) {
		Color &col =  _rt->_scene.getImage().at(raybundle.x,raybundle.y);// raybundle.color;
		col = Color(0,0,0);
		//trace all rays and average the resulting color
		int s = raybundle.sampling;
		for(int n=0; n<s; ++n)
			for(int m=0; m<s; ++m){
				//calculate right ray;
				Ray &ray = raybundle.r;
				//std::cout<< real(n)/s <<std::endl;
				//std::cout<< real(m)/s <<std::endl;

				real rightShift = _rt->_scene.getWidth()*(raybundle.x+( (n%2==0?.5:-.5)*real(n)/s))/_rt->_scene.getImage().getWidth();
				real downShift = _rt->_scene.getHeight()*(raybundle.y+( (m%2==0?.5:-.5)*real(m)/s))/_rt->_scene.getImage().getHeight();
				Vector3 &dir = ray.Direction();
				dir = _rt->_scene.getUpperLeft()
							+ _rt->_scene.getRight() * rightShift
							+ _rt->_scene.getDown() * downShift
							- _rt->_scene.getEye();
				dir.normalize();
				col += _rt->traceRay(ray);
				}

		col/=(s*s);
		//highlight edges
		if(s>1&&_showEdges)
			col = Color(1,0,0);
	}
};

