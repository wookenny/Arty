#pragma once

#include "Color.h"
#include "Obstacle.h"
#include <cmath>



int part(real x,real f){
	return (int)(ceil( x/f));
}

class ChessboardTexture: public Texture{

	private:
		const Color _a;
		const Color _b;
		const real _freq1;
		const real _freq2;
		const real _freq3;
	public:
		ChessboardTexture(): _a(Color(1,1,1)),_b(Color(0,0,0)),_freq1(0.1),_freq2(0.1),_freq3(0.1){}
		ChessboardTexture(Color a, Color b, real f1, real f2,real f3):_a(a),_b(b),_freq1(f1),_freq2(f2),_freq3(f3){}	
		Color at(const IntersectionCompound& ic) const{ 
				//3D texture:
				if(ic.px <0 or ic.py<0){
					if (( part(ic.hitpoint[0],_freq1) + part(ic.hitpoint[1],_freq1) + part(ic.hitpoint[2],_freq1) )%2==0)
						return _a;
					return _b; 
				}
				//2D texture			
				if ( (part(ic.px,_freq1) + part(ic.py,_freq2) )%2 == 0)
					return _a;
				return _b; 
		}
};
