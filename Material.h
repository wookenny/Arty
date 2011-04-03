#pragma once

#include "common.h"
#include "Color.h"

class Material{
	private:
		real k_phong;
		real k_transparancy;
		real k_specular;
		real k_ambient;
		real k_diffuse;
		real k_shininess;	
		
		Color _color;	
		
	public:
		//cstr
		Material(real p=0.5, real t=0, real s=0, real a=0.1, real d=0.7, real sh=30, const Color& c=Color(0,1,0)):k_phong(p),k_transparancy(t),k_specular(s),k_ambient(a),k_diffuse(d),k_shininess(sh),_color(c){}
		//getter
		real getPhong() const{ return k_phong; }
		real getTransparancy() const{ return k_transparancy; }
		real getSpecular() const{ return k_specular; }
		real getAmbient() const{ return k_ambient;}
		real getDiffuse() const{ return k_diffuse;}
		real getShininess() const{ return k_shininess; }
		virtual Color getColor(real, real) const{ return _color; }
			
		
	
		//setter
		void setPhong(real p){ k_phong = p;}
		void setTransparancy(real t){ k_transparancy = t;}
		void setSpecular(real s){ k_specular = s;}
		void setAmbient(real a){ k_ambient = a;}
		void setDiffuse(real d){ k_diffuse = d;}
		void setShininess(real s){ k_shininess = s;}
		void setColor(const Color& c){ _color = c; }
		//get color
		

};
