#pragma once

class Color;
/** base class for all texture classes **/
class Texture{

	public:
		virtual Color at(real x, real y) const = 0;
 		virtual ~Texture() {}
};
