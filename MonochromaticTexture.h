#pragma once

#include "Color.h"

class MonochromaticTexture: public Texture{

	private:
		const Color _c;
	public:
		MonochromaticTexture(): _c(Color(1,1,1)){}
		MonochromaticTexture(real r, real g, real b):_c(Color(r,g,b)){}
		MonochromaticTexture(const Color c):_c(c){}
		Color at(real x, real y) const{ return _c; }
};
