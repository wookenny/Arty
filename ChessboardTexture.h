#pragma once

#include "Color.h"
#include <cmath>

class ChessboardTexture: public Texture{

	private:
		const Color _a;
		const Color _b;
		const real _freq;
	public:
		ChessboardTexture(): _a(Color(1,1,1)),_b(Color(0,0,0)),_freq(0.1){}
		ChessboardTexture(Color a, Color b, real f):_a(a),_b(b),_freq(f){}	
		Color at(real x, real y) const{ if ((int)(ceil( x/_freq) + ceil(y/_freq) )%2==0)
											return _a;
										return _b; }
};
