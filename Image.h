#pragma once
#include <vector>
#include <string>
#include <iostream>

#include "Color.h"
#include "common.h"

class Image{

private:
	std::vector<std::vector<Color> > _data;	

	static real _gamma;

	void gammaCorrection();
	void exposureCorrection();

public:	

	//constructors
	Image(unsigned int width = 800,unsigned int height = 600);
	Image(const Image& img){ _data = img._data; }

	//getter
	unsigned int getWidth() const{ return _data.size(); }	
	unsigned int getHeight() const{ return _data.at(0).size(); }
	static real getGamma() { return _gamma;}	

	//setter
	static void setGamma(real g){ _gamma = g;}

	//setter+getter:
	Color& at(unsigned int i, unsigned int j){ return _data.at(i).at(j);}

	//misc methods	
	void save(const std::string& filename);
	std::vector<std::vector<bool> > findEdges() const;

};

