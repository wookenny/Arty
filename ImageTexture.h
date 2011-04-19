#pragma once

#include "Color.h"
#include "Image.h"

class ImageTexture: public Texture{

	private:
		const Image* _img;
	public:
		ImageTexture(const std::string name):_img(new Image(name)){}
		~ImageTexture(){ delete _img;}
		Color at(real x, real y) const{
			return _img->relAt(x,y);			
		}
};
