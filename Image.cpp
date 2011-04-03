#include "Image.h"
#include "pngwriter.h"

//for writing jpgs


//definition of static member variable
real Image::_gamma = 2.2;

Image::Image(unsigned int width, unsigned int height){
	_data = std::vector<std::vector<Color> >( width, std::vector<Color>( height, Color(0,0,0) ) );
}

void Image::gammaCorrection(){
	for(unsigned int i=0; i<_data.size();++i)
		for(unsigned int j=0; j<_data.at(i).size();++j)
			_data.at(i).at(j).gammaCorrection(_gamma);
}

void Image::exposureCorrection(){
	for(unsigned int i=0; i<_data.size();++i)
		for(unsigned int j=0; j<_data.at(i).size();++j)
			_data.at(i).at(j).exposureCorrection();
}
	

void Image::save(const std::string& filename){
	/** shift colors **/
	//exposureCorrection();  //not quite good, somethiong has to be done
	gammaCorrection();	

	pngwriter writer(getWidth(), getHeight(), 0.0, filename.c_str());
	for(unsigned int i=0; i<_data.size();++i)
		for(unsigned int j=0; j<_data.at(i).size();++j){
			Color c = _data.at(i).at(j);
			writer.plot(i,getHeight()-1-j, c.getRed(), c.getGreen(), c.getBlue()); 
		}	
	writer.write_png();
}


std::vector<std::vector<bool> > Image::findEdges() const{
	std::vector<std::vector<bool> > edges(_data.size(),std::vector<bool>(_data.at(0).size(),false));
	// apply laplce operator 
	unsigned int n,m;
	real thresh = 0.05;
	n = edges.size()-1; m = edges.at(0).size()-1;
	for(unsigned int i = 0; i <= n; ++i){
		for(unsigned int j = 0; j <= m; ++j){
			if(i==0 && j==0)
				edges.at(0).at(0) = ( (_data.at(0).at(1)+_data.at(1).at(0)-2*_data.at(0).at(0)).max() > thresh  );						
			else if(i==0 && j==m)
				edges.at(0).at(m) = ( (_data.at(0).at(m-1)+_data.at(1).at(m)-2*_data.at(0).at(m)).max() > thresh  );
			else if(i==n && j==0)
				edges.at(n).at(0) = ( (_data.at(n).at(1)+_data.at(n-1).at(0)-2*_data.at(n).at(0)).max() > thresh  );
			else if(i==n && j==m)
				edges.at(n).at(m) = ( (_data.at(n).at(m-1)+_data.at(n-1).at(m)-2*_data.at(n).at(m)).max() > thresh  );
			else if(i==0)
				edges.at(0).at(j) = ( (_data.at(0).at(j-1)+_data.at(0).at(j+1)-2*_data.at(0).at(j)).max() > thresh  );			
			else if(j==0)
				edges.at(i).at(0) = ( (_data.at(i-1).at(0)+_data.at(i+1).at(0)-2*_data.at(i).at(0)).max() > thresh  );	
			else if(i==n)
				edges.at(n).at(j) = ( (_data.at(n).at(j-1)+_data.at(n).at(j+1)-2*_data.at(n).at(j)).max() > thresh  );
			else if(j==m)
				edges.at(i).at(m) = ( (_data.at(i-1).at(m)+_data.at(i+1).at(m)-2*_data.at(i).at(m)).max() > thresh  );	
			else
				edges.at(i).at(j) = ( (_data.at(i-1).at(j)+_data.at(i+1).at(j)+_data.at(i).at(j-1)+_data.at(i).at(j+1)-4*_data.at(i).at(j)).max() > 2*thresh  );	
			
			//for debugging: set all to 1
			//edges.at(i).at(j)=true;
		}
	}
	//widen selection
	std::vector<std::vector<bool> > additional(_data.size(),std::vector<bool>(_data.at(0).size(),false));
	for(unsigned int i = 1; i <= n-1; ++i)
		for(unsigned int j = 1; j <= m-1; ++j)
			if( edges.at(i).at(j+1) or edges.at(i).at(j-1) or edges.at(i+1).at(j) or edges.at(i+1).at(j) 
			   or edges.at(i+1).at(j+1) or edges.at(i-1).at(j-1) or edges.at(i-1).at(j+1) or edges.at(i+1).at(j-1))
				additional.at(i).at(j) = true;	
		
	
	for(unsigned int i = 1; i <= n-1; ++i)
		for(unsigned int j = 1; j <= m-1; ++j)
			if( additional.at(i).at(j) )
				edges.at(i).at(j) = true;

	return edges;
}

	
