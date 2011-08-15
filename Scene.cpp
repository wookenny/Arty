#include "Scene.h"
#include "pugixml.hpp"
#include "pugiconfig.hpp"
#include "MonochromaticTexture.h"
#include "ImageTexture.h"
#include "ChessboardTexture.h"
#include "Obstacle.h"
#include "Sphere.h"
#include "Triangle.h"

#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <iostream>
#include <fstream>






Scene::Scene( const Scene &s):_eye(s._eye), _down(s._down), _right(s._right),
				_upperLeftCorner(s._upperLeftCorner), _width(s._width), _height(s._height),
				_pixelPerUnit(s._pixelPerUnit), _tracedImage(s._tracedImage),
				_maxRayDepth(s._maxRayDepth), _superSampling(s._superSampling),
				_objects(s._objects), _textures(s._textures), _lights(s._lights),
				_vertices(s._vertices), _materials(s._materials), _ambient(s._ambient){
	_copy = true;

}


Scene::~Scene(){
	if(_copy)//nothing to delete if this is a copy
		return;
	//delete all objects since they were loaded dynamically
	for( std::vector<Obstacle*>::iterator i = _objects.begin(); i != _objects.end(); ++i )
    		delete *i;
	//now the textures
	for( std::vector<Texture*>::iterator i = _textures.begin(); i != _textures.end(); ++i )
    		delete *i;
}

void Scene::loadDefaultScene(){

		_eye = Vector3(eps,eps,eps-2);//not (0,0,0) because of rounding errors
 		_right = Vector3(1,0,0);
 		_down = Vector3(0,-1,0);
		_upperLeftCorner = Vector3(-1.,1.,1);
		_width = 2.; //size of viewing window
		_height = 2.;
		_pixelPerUnit = 400;
		_superSampling = 4;
		_tracedImage = Image(_width*_pixelPerUnit, _height*_pixelPerUnit);
		_tracedImage.setGamma(2.2);
		_maxRayDepth = 4;
		_ambient = 0.1*Color(1,1,1);

		//KD-Tree objects
		Material mat1,mat2,mat3,mat4,mat5;
		Texture *red,*green,*white,*blueish,*chessboard;
		red = new MonochromaticTexture(1,0,0);
		green = new MonochromaticTexture(0,1,0);
		white = new MonochromaticTexture(1,1,1);
		blueish = new MonochromaticTexture(0.0,0.0,0.8);
		chessboard = new ChessboardTexture();

		mat1.setTexture(red);
		mat2.setTexture(green);
		mat3.setTexture(white);
		mat4.setTexture(blueish);
		mat5.setTexture(chessboard);

		mat4.setSpecular(0.2);
		mat4.setShininess(64);
		_materials["mat1"] = mat1;_materials["mat2"] = mat2;
		_materials["mat3"] = mat3;_materials["mat4"] = mat4;
		_materials["mat5"] = mat5;


		_vertices["LUH"] = Vector3(-1,-1,3);
		_vertices["RUH"] = Vector3(1,-1,3);
		_vertices["ROH"] = Vector3(1,1,3);
		_vertices["LOH"] = Vector3(-1,1,3);
		_vertices["LUV"] = Vector3(-1,-1,1);
		_vertices["RUV"] = Vector3(1,-1,1);
		_vertices["ROV"] = Vector3(1,1,1);
		_vertices["LOV"] = Vector3(-1,1,1);


		//hinten
		Triangle* t = new Triangle(_vertices["LUH"],_vertices["ROH"],_vertices["RUH"]);
		t->setMaterial(&_materials["mat3"]);
		_objects.push_back(t);//pointers because of polymorphism

		t = new Triangle(_vertices["LUH"],_vertices["LOH"],_vertices["ROH"]);
		t->setMaterial(&_materials["mat3"]);
		_objects.push_back(t);//pointers because of polymorphism

		//links
		t = new Triangle(_vertices["LUV"],_vertices["LOH"],_vertices["LUH"]);
		t->setMaterial(&_materials["mat1"]);
		_objects.push_back(t);//pointers because of polymorphism
		t = new Triangle(_vertices["LUV"],_vertices["LOV"],_vertices["LOH"]);
		t->setMaterial(&_materials["mat1"]);
		_objects.push_back(t);//pointers because of polymorphism
		//rechts
		t = new Triangle(_vertices["RUV"],_vertices["RUH"],_vertices["ROH"]);
		t->setMaterial(&_materials["mat2"]);
		_objects.push_back(t);//pointers because of polymorphism
		t = new Triangle(_vertices["RUV"],_vertices["ROH"],_vertices["ROV"]);
		t->setMaterial(&_materials["mat2"]);
		_objects.push_back(t);//pointers because of polymorphism

		//unten
		t = new Triangle(_vertices["LUV"],_vertices["LUH"],_vertices["RUH"]);
		t->setMaterial(&_materials["mat5"]);
		t->setTextureCoords("0 0 1 0 1 1");
		_objects.push_back(t);//pointers because of polymorphism
		t = new Triangle(_vertices["LUV"],_vertices["RUH"],_vertices["RUV"]);
		t->setMaterial(&_materials["mat5"]);
		t->setTextureCoords("0 0 1 1 0 1");
		_objects.push_back(t);//pointers because of polymorphism


				//new sphere
		Sphere* s = new Sphere(Vector3(.3,-1+.3,2),0.3);
		s->setMaterial(&_materials["mat4"]);
		_objects.push_back(s);//pointers because of polymorphism


		Vector3 light(0.,1,2);
		//s = new Sphere(light,0.1);
		//s->setMaterial(&_materials["mat3"]);
		//_objects.push_back(s);//pointers because of polymorphism

		//configure square shaped light
		Lightsource l = Lightsource(light, 3, Color(1,1,1));
		l.setType(Lightsource::Type::Square);
		//l.setType(Lightsource::Type::Point);
		l.setSize(0.5);
		l.setDirection1(Vector3(1,0,0));
		l.setDirection2(Vector3(0,0,1));
		_lights.push_back(l);



}


void Scene::loadScene(const std::string &xmlfile){
	std::cout<<"loading scene "<<xmlfile<<std::endl;

		std::string path = "";
		auto found = xmlfile.rfind("/");
  		if (found!=std::string::npos)
			path = xmlfile.substr(0,found+1);


		//code_load_file
        pugi::xml_document doc;
    	pugi::xml_parse_result result = doc.load_file(xmlfile.c_str());
	//supress warning:
	if(not result){ std::cout<<"WARNING: parsing error!"<<std::endl;}
	//TODO: give a meaningfull message about the parsing


    //getting general settings:
   	pugi::xml_node child = doc.child("Scene").child("Boundaries");
   	//additional settings:
	_maxRayDepth = 4;
	for (pugi::xml_node_iterator it = child.begin(); it != child.end(); ++it){
		pugi::xml_attribute_iterator ait = it->attributes_begin();
		std::string name = ait->value();
		if ( name == std::string("Eye") ){
			++ait;
			_eye = Vector3(ait->value());
 		}
		else if ( name == std::string("Right") ){
			++ait;
			_right = Vector3(ait->value());
 		}
		else if ( name == std::string("Down") ){
			++ait;
			_down = Vector3(ait->value());
 		}
		else if ( name == std::string("UpperLeftCorner") ){
			++ait;
			_upperLeftCorner = Vector3(ait->value());
 		}
		else if ( name == std::string("Width") ){
			++ait;
			_width = ait->as_float();
		}
		else if ( name == std::string("Height") ){
			++ait;
			_height = ait->as_float();
		}
		else if ( name == std::string("PixelPerUnit") ){
			++ait;
			_pixelPerUnit = ait->as_int();
		}
		else if ( name == std::string("Supersampling") ){
			++ait;
			_superSampling = ait->as_int();
		}
		else if( name == std::string("MaxRayDepth") )
			++ait;
			_maxRayDepth = ait->as_int();
	}
	_tracedImage = Image(_width*_pixelPerUnit, _height*_pixelPerUnit);
	_tracedImage.setGamma(2.2);


	//getting light settings:
       child = doc.child("Scene").child("Lights");
	for (pugi::xml_node_iterator it = child.begin(); it != child.end(); ++it){
		std::string type = it->attribute("Type").value();
		if(type == "ambient"){
			_ambient = 0.05*Color(it->attribute("Color").value());
		}else if( type == "point"){
			Vector3 lightpos(it->attribute("Position").value());
			_lights.push_back(Lightsource(lightpos, it->attribute("Intensity").as_float(),
					Color(it->attribute("Color").value())));

		}else{
			std::cout<<"Warning: light of type "<<type<<" not supported!"<<std::endl;
		}

	}

	//getting materials:
        child = doc.child("Scene").child("Materials");
	for (pugi::xml_node_iterator it = child.begin(); it != child.end(); ++it){
		std::string name = it->attribute("Name").value();
		Material tmpmat;
		if( it->attribute("Color") ){
			//TODO: get rid of the memory
			Texture *tex = new MonochromaticTexture(Color(it->attribute("Color").value()));
			tmpmat.setTexture(tex);
		}
		if( it->attribute("Specular") )
			tmpmat.setSpecular(it->attribute("Specular").as_float());
		if( it->attribute("Shininess") )
			tmpmat.setShininess(it->attribute("Shininess").as_int());
		if( it->attribute("Reflection") )
			tmpmat.setReflection(it->attribute("Reflection").as_float());
		if( it->attribute("Texture") ){
			//TODO get rid of memory
			Texture *tex = new ImageTexture(path + it->attribute("Texture").value());
			tmpmat.setTexture(tex);
		}
		if( it->attribute("ProceduralTexture") ){
			if(it->attribute("ProceduralTexture").value()== std::string("chessboard")){
				Texture *tex = new ChessboardTexture( it->attribute("Params").value() );
				tmpmat.setTexture(tex);
			}

		}
		_materials[name] = tmpmat;

	}

	//get implicit objects
	child = doc.child("Scene").child("Geometry").child("Implicits");
	for (pugi::xml_node_iterator it = child.begin(); it != child.end(); ++it){
		std::string type = it->attribute("Type").value();
		if(type == "Sphere"){
			Sphere* s = new Sphere( Vector3(it->attribute("Position").value()),
						it->attribute("Radius").as_float());
			s->setMaterial(&_materials[it->attribute("Material").value()]);
			_objects.push_back(s);//pointers because of polymorphism
		}else{
			std::cout<<"Warning: implicit object with type "<<type<<" not supported!"<<std::endl;
		}
	}

	//get vertices
	child = doc.child("Scene").child("Geometry").child("Points");
	for (pugi::xml_node_iterator it = child.begin(); it != child.end(); ++it){
		std::string name = it->attribute("Name").value();
		_vertices[name] = Vector3(  it->attribute("Position").value() );
	}

	//get triangles
	child = doc.child("Scene").child("Geometry").child("Triangles");
	for (pugi::xml_node_iterator it = child.begin(); it != child.end(); ++it){
		std::string name = it->attribute("Name").value();

		Triangle *t = new Triangle( 	_vertices[it->attribute("A").value()],
						_vertices[it->attribute("B").value()],
						_vertices[it->attribute("C").value()]);
		t->setMaterial(&_materials[ it->attribute("Material").value() ]);

		if( it->attribute("TextureCoords") )
			t->setTextureCoords(it->attribute("TextureCoords").value());

		_objects.push_back(t);
	}

	//get off files
	child = doc.child("Scene").child("Geometry").child("OFF");
	for (pugi::xml_node_iterator it = child.begin(); it != child.end(); ++it){
		std::string name = it->attribute("Name").value();
		loadOFF_File(path + name,
						it->attribute("x1").as_float(),it->attribute("x2").as_float(),
						it->attribute("y1").as_float(),it->attribute("y2").as_float(),
						it->attribute("z1").as_float(),it->attribute("z2").as_float(),
						it->attribute("Material").value());
	}
}

void Scene::loadOFF_File(const std::string &str, real x_min,
			real x_max, real y_min, real y_max, real z_min, real z_max, std::string material){
	//open file
	std::ifstream file;
	file.open(str.c_str());
	std::string line;

	//1th line: OFF
	file >> line;

	//2th line: #vertices #faces #edges
	file >> line;
	long numVertices = atol(line.c_str());
	file >> line;
	long numFaces = atol(line.c_str());
	file >> line;

	//all vertices (x y z)
	real x1,x2,y1,y2,z1,z2; x1=x2=y1=y2=z1=z2=0;
	for(long i = 0; i<numVertices;++i){
		real x,y,z;
		file >> line; x =  -real(atof(line.c_str()));
		file >> line; y =  real(atof(line.c_str()));
		file >> line; z =  -real(atof(line.c_str()));

		if(0==i){//init values
			x1 = x2 = x;
			y1 = y2 = y;
			z1 = z2 = z;
		}else{//max/min
			x1 = std::min(x1,x); x2 = std::max(x2,x);
			y1 = std::min(y1,y); y2 = std::max(y2,y);
			z1 = std::min(z1,z); z2 = std::max(z2,z);
		}
		//add new vertex
		_vertices[ str + boost::lexical_cast<std::string>(i) ] = Vector3(x,y,z);

	}

	//shift all points
	real scale = (x_max - x_min)/(x2-x1);
	scale = std::min(scale, (y_max - y_min)/(y2-y1) );
	scale = std::min(scale, (z_max - z_min)/(z2-z1) );

	real x_shift = x_min - x1*scale;
	real y_shift = y_min - y1*scale;
	real z_shift = z_min - z1*scale;

	for(long i = 0; i<numVertices;++i){
		//scale point
		std::string name = str + boost::lexical_cast<std::string>(i);
		_vertices[ name ] *= scale;
		//shift point
		_vertices[ name ] += Vector3(x_shift,y_shift,z_shift);
		if(0==i){
			x1 = x2 =  	_vertices[ name ][0];
			y1 = y2 = 	_vertices[ name ][1];
			z1 = z2 = 	_vertices[ name ][2];
		}else{
			x1 = std::min(x1,_vertices[ name ][0]); x2 = std::max(x2,_vertices[ name ][0]);
			y1 = std::min(y1,_vertices[ name ][1]); y2 = std::max(y2,_vertices[ name ][1]);
			z1 = std::min(z1,_vertices[ name ][2]); z2 = std::max(z2,_vertices[ name ][2]);
		}
	}


	std::vector< boost::tuple<long,long,long> > triangleList;
	std::vector<Vector3> normals(numVertices, Vector3(0,0,0));
	for(long i = 0; i<numFaces;++i){
		int n;
		file >> line;
		n = atoi(line.c_str());

		if(n==4){
			long a,b,c,d;
			file >> line; a = atol(line.c_str());
			file >> line; b = atol(line.c_str());
			file >> line; c = atol(line.c_str());
			file >> line; d = atol(line.c_str());
			triangleList.push_back( boost::make_tuple(a,b,c) );
			triangleList.push_back( boost::make_tuple(a,c,d) );
			//create Normal
			const Vector3& v1 = _vertices[ str + boost::lexical_cast<std::string>(a) ];
			const Vector3& v2 = _vertices[ str + boost::lexical_cast<std::string>(b) ];
			const Vector3& v3 = _vertices[ str + boost::lexical_cast<std::string>(c) ];
			Vector3 n = (v2-v1).cross(v3-v1);
			n.normalize();
			normals[a] += n;
			normals[b] += n;
			normals[c] += n;
			normals[d] += n;

		}else if(3==n){
			long a,b,c;
			file >> line; a = atol(line.c_str());
			file >> line; b = atol(line.c_str());
			file >> line; c = atol(line.c_str());
			triangleList.push_back( boost::make_tuple(a,b,c) );
			//create Normal
			const Vector3& v1 = _vertices[ str + boost::lexical_cast<std::string>(a) ];
			const Vector3& v2 = _vertices[ str + boost::lexical_cast<std::string>(b) ];
			const Vector3& v3 = _vertices[ str + boost::lexical_cast<std::string>(c) ];
			Vector3 n = (v2-v1).cross(v3-v1);
			n.normalize();
			normals[a] += n;
			normals[b] += n;
			normals[c] += n;

		}else{
			std::cout<<"WARNING: "<< n <<" vertices per face not supported!"<<std::endl;
			return;
		}
	}

	//actually create all faces and all cor. normals
	for(unsigned int i=0; i<triangleList.size(); ++i){
		long a = triangleList[i].get<0>();
		long b = triangleList[i].get<1>();
		long c = triangleList[i].get<2>();
		Triangle *t = new Triangle( 	_vertices[str + boost::lexical_cast<std::string>(a)],
						_vertices[str + boost::lexical_cast<std::string>(b)],
						_vertices[str + boost::lexical_cast<std::string>(c)]);
		t->setMaterial(&_materials[ material ]);
		t->setNormals( normals[a], normals[b], normals[c] );
		_objects.push_back(t);
	}


}

