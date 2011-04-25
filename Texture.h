#pragma once

class IntersectionCompound;
class Color;
/** base class for all texture classes **/
class Texture{

	public:
		virtual Color at(const IntersectionCompound&) const = 0;
 		virtual ~Texture() {}
};
