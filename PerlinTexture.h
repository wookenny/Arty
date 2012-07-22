#pragma once

#include "PerlinNoise.h"
#include "Color.h"
#include "Obstacle.h"


class PerlinTexture: public Texture{
	public:

	  	// De-/Constructor
		PerlinTexture(int x = 10, int y = 10, int z = 10,
					double p=0.1, double f=0.1, double a=0.1, int o=0, int r=0):
					_X(x),_Y(y),_Z(z),noiseRed(p,f,a,o,r),
					noiseGreen(p,f,a,o,r+1),noiseBlue(p,f,a,o,r+2){} 

		PerlinTexture(std::string s):_X(500),_Y(500),_Z(500){
				std::vector<std::string> v = split(s, ';');
				if(v.size() >0)  _X = std::stoi(v[0]);
				if(v.size() >1)  _Y = std::stoi(v[1]);
				if(v.size() >2)  _Z = std::stoi(v[2]);


				double p=0.1; double f=0.1; double a=0.1; int o=0; int r=0;
				if(v.size() >3) p = std::stod(v[3]);
				if(v.size() >4) f = std::stod(v[4]);
				if(v.size() >5) a = std::stod(v[5]);
				if(v.size() >6) o = std::stoi(v[6]);
				if(v.size() >7) r = std::stod(v[7]);
				noiseRed =   PerlinNoise(p,f,a,o,r);
				noiseGreen = PerlinNoise(p,f,a,o,r+1000);
				noiseBlue =  PerlinNoise(p,f,a,o,r+2000);
		}


		//TODO: 3 different version for r,g,b
		~PerlinTexture(){}  

		Color at(const IntersectionCompound& ic) const{
			//scale ewverything correctly
			//2D or 3D texture?
			//multipoly with X,Y,Z -> correct request/query
			return Color(   noiseRed.GetValue(_X*ic.px,_Y*ic.py),
					noiseGreen.GetValue(_X*ic.px,_Y*ic.py),
					noiseBlue.GetValue(_X*ic.px,_Y*ic.py) );
		};
  
	private:
		int _X;
		int _Y;
		int _Z;
		PerlinNoise noiseRed;
		PerlinNoise noiseGreen;
		PerlinNoise noiseBlue;
};
