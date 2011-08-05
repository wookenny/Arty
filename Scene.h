#pragma once

#include <boost/unordered_map.hpp>
#include <string>
#include <vector>
#include "Color.h"
#include "Obstacle.h"
#include "Material.h"
#include "Vector3.h"
#include "Lightsource.h"
#include "common.h"


//container class for parts to render

class Scene{

	private:
		bool _copy;

		//general settings:
		Vector3 _eye, _down, _right, _upperLeftCorner;
		real _width, _height;//size of viewing window
		real _pixelPerUnit;
		Image _tracedImage;
		int _maxRayDepth;
		int _superSampling; //calculate s x s Rays for each pixel on an edge

		//KD-Tree objects
		std::vector<Obstacle*> _objects;//pointers because of polymorphism
		std::vector<Texture*> _textures;//pointers because of polymorphism

		std::vector<Lightsource> _lights;
		boost::unordered_map<std::string, Vector3> _vertices;
		boost::unordered_map<std::string, Material> _materials;


		//ambient lightning
		Color _ambient;


		void setLightsources(const std::vector<Lightsource>& l){ _lights = l; }
		void setObjects(const std::vector<Obstacle*>& o){ _objects =o;}

	public:
		Scene():_copy(false){}
		Scene( const Scene &s);
		~Scene();
		void loadOFF_File(const std::string &str, real x_min, real x_max,
				real y_min, real y_max, real z_min, real z_max, std::string material);


		void loadScene(const std::string &xmlfile);
		void loadDefaultScene();


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



		//getter
		//objects
		unsigned int getNumObjects() const{ return _objects.size(); }
		const Obstacle* const getObject(unsigned int i) const{ return _objects[i]; }
		//lightsources
		unsigned int getNumLights() const{ return _lights.size(); }
		const Lightsource & getLight(unsigned int i) const{ return _lights[i]; }
		const Color& getAmbient() const{ return _ambient;}
		const Vector3 &getUpperLeftCorner() const{ return _upperLeftCorner;}


		const Vector3 &getEye() const{ return _eye;}
		const Vector3 &getDown() const{ return _down;}
		const Vector3 &getRight() const{ return _right;}
		const Vector3 &getUpperLeft() const{ return _upperLeftCorner;}

		real getWidth() const{ return _width;}
		real getHeight() const{ return _height;}
		real getPixelPerUnit() const{ return _pixelPerUnit; }
		Image& getImage() {return  _tracedImage;}
		const Image& getImage() const {return  _tracedImage;}
		int getMaxRayDepth() const {return _maxRayDepth; }
		int getSampling() const { return _superSampling; }


};

