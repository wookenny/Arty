#pragma once

#include "Color.h"
#include "Image.h"
#include "Obstacle.h"

class ImageTexture: public Texture{

	private:
		const Image* _img;
	public:
		ImageTexture(const std::string name):_img(new Image(name)){}
		~ImageTexture(){ delete _img;}
		Color at(const IntersectionCompound& ic) const{
			return _img->relAt(ic.px,ic.py);			
		}
};
