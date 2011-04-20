#pragma once

#include "Color.h"
#include "Obstacle.h"
#include <cmath>

class ChessboardTexture: public Texture{

	private:
		const Color _a;
		const Color _b;
		const real _freq1;
		const real _freq2;
	public:
		ChessboardTexture(): _a(Color(1,1,1)),_b(Color(0,0,0)),_freq1(0.1),_freq2(0.1){}
		ChessboardTexture(Color a, Color b, real f1, real f2):_a(a),_b(b),_freq1(f1),_freq2(f2){}	
		Color at(const IntersectionCompound& ic) const{ if ((int)(ceil( ic.px/_freq1) + ceil(ic.py/_freq2) )%2==0)
											return _a;
										return _b; }
};
