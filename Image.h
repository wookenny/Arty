#pragma once
#include <vector>
#include <string>
#include <iostream>

#include "Color.h"
#include "common.h"
#include <string>
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

	Image(const std::string file);

	//getter
	unsigned int getWidth() const{ return _data.size(); }	
	unsigned int getHeight() const{ return _data[0].size(); }
	static real getGamma() { return _gamma;}	

	//setter
	static void setGamma(real g){ _gamma = g;}

	//setter+getter:
	const Color& at(unsigned int i, unsigned int j) const{ return _data[i][j];}
	Color& at(unsigned int i, unsigned int j){ return _data[i][j];}
	
	const Color& relAt(real i, real j) const{
			int ni  = (int)(i*_data.size())%_data.size();
			int nj  =  (int)(j*_data[0].size())%_data[0].size();
			return _data[ni][nj];}

	Color& relAt(real i, real j){
			int ni  = (int)(i*_data.size())%_data.size();
			int nj  =  (int)(j*_data[0].size())%_data[0].size();
			return _data[ni][nj];}

	//misc methods	
	void save(const std::string& filename);
	std::vector<std::vector<bool> > findEdges() const;

};

