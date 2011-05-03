#pragma once

#include "PerlinNoise.h"
#include "Color.h"
#include "Obstacle.h"


class PerlinTexture: public Texture{
	public:

	  	// De-/Constructor
		PerlinTexture(int x = 10, int y = 10, int z = 10
					double p, double f, double a, int o, int r):
					_X(x),_Y(y),_Z(z),noiseRed(p,f,a,o,r),
					noiseGreen(p,f,a,o,r+1),noiseBlue(p,f,a,o,r+2){} 
		//TODO: 3 different version for r,g,b
		~PerlinTexture(){}  

		Color at(const IntersectionCompound&){
			//scale ewverything correctly
			//2D or 3D texture?
			//multipoly with X,Y,Z -> correct request/query
			return Color(noiseRed,0,0);
		};
  
	private:
		const int _X;
		const int _Y;
		const int _Z
		const PerlinNoise noiseRed;
		const PerlinNoise noiseGreen;
		const PerlinNoise noiseBlue;
};
