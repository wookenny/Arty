#include "Image.h"
#include "pngwriter.h"
#include <cassert>
//for writing jpgs


//definition of static member variable
real Image::_gamma = 2.2;

Image::Image(unsigned int width, unsigned int height){
	_data = std::vector<std::vector<Color> >( width, std::vector<Color>( height, Color(0,0,0) ) );
}


Image::Image(const std::string file){

	pngwriter img(1/*width*/, 1/*height*/, 0.0/*bg color*/, file.c_str());
	img.readfromfile( file.c_str() );

	_data = std::vector<std::vector<Color> >( img.getwidth() ,
											std::vector<Color>( img.getheight(), Color(0,0,0) ) );


	for(unsigned int i=0; i<_data.size();++i){
		for(unsigned int j=0; j<_data[i].size();++j){
			Color c(real(img.read(i+1,j+1,1))/65536.,
					real(img.read(i+1,j+1,2))/65536.,
					real(img.read(i+1,j+1,3))/65536. );
			_data[i][j] = c;
		}
	}
}


void Image::gammaCorrection(){
	for(unsigned int i=0; i<_data.size();++i)
		for(unsigned int j=0; j<_data[i].size();++j)
			_data[i][j].gammaCorrection(_gamma);
}

real Image::findExposureCoefficient() const{
//similiar to the code shown in a web tutorial(without further explanations)
/*float AutoExposure(scene & myScene)
{
    #define ACCUMULATION_SIZE 16
    float exposure = -1.0f;
    float accumulationFactor = float(max(myScene.sizex, myScene.sizey));

    accumulationFactor = accumulationFactor / ACCUMULATION_SIZE;
    color mediumPoint = 0.0f;
    const float mediumPointWeight = 1.0f / (ACCUMULATION_SIZE*ACCUMULATION_SIZE);
    for (int y = 0; y < ACCUMULATION_SIZE; ++y) {
        for (int x = 0 ; x < ACCUMULATION_SIZE; ++x) {
            ray viewRay = { {float(x) * accufacteur,
                             float(y) * accufacteur, -1000.0f},
                            { 0.0f, 0.0f, 1.0f}};
            color currentColor = addRay (viewRay, myScene);
            float luminance = 0.2126f * currentColor.red
                            + 0.715160f * currentColor.green
                            + 0.072169f * currentColor.blue
            mediumPoint += mediumPointWeight * (luminance * luminance);
        }
    }

    float mediumLuminance = sqrtf(mediumPoint);
    if (mediumLuminance > 0.001f)
    {
        // put the medium luminance to an intermediate gray value
        exposure = logf(0.6f) / mediumLuminance;
    }

    return exposure;
*/
	real exposure = -1;
	real mediumPoint = 0.0f;
	for(unsigned int i=0; i<_data.size();++i)
		for(unsigned int j=0; j<_data[i].size();++j){
				mediumPoint += 0.2126   + _data[i][j].getRed();
				mediumPoint += 0.715160 + _data[i][j].getGreen();
				mediumPoint += 0.072169 + _data[i][j].getBlue();
		}
	mediumPoint /= getWidth()*getHeight();

	real mediumLuminance = sqrt(mediumPoint);
	if (mediumLuminance > eps)
    {
        // put the medium luminance to an intermediate gray value
        exposure = log(0.6f) / mediumLuminance;
    }
    return exposure;
}

void Image::exposureCorrection(){
	//get the exposure correction coefficent
	real exposure = -1;
	//real exposure = findExposureCoefficient();

	//correct every single color
	for(unsigned int i=0; i<_data.size();++i)
		for(unsigned int j=0; j<_data[i].size();++j)
			_data[i][j].exposureCorrection(exposure);
}


void Image::save(const std::string& filename){
	/** shift colors **/
	exposureCorrection();  //not quite good, somethiong has to be done
	//should be an auto correction t yield nicely exposed images!
	gammaCorrection();

	pngwriter writer(getWidth(), getHeight(), 0.0, filename.c_str());
	for(unsigned int i=0; i<_data.size();++i)
		for(unsigned int j=0; j<_data[i].size();++j){
			const Color &c = _data[i][j];
			writer.plot(i+1,getHeight()-j, c.getRed(), c.getGreen(), c.getBlue());
		}
	writer.write_png();
}


std::vector<std::vector<bool> > Image::findEdges() const{
	std::vector<std::vector<bool> > edges(_data.size(),std::vector<bool>(_data[0].size(),false));
	// apply laplce operator
	unsigned int n,m;
	real thresh = 0.05;
	n = edges.size()-1; m = edges[0].size()-1;
	for(unsigned int i = 0; i <= n; ++i){
		for(unsigned int j = 0; j <= m; ++j){
			if(i==0 && j==0)
				edges[0][0] = ( (_data[0][1]+_data[1][0]-2*_data[0][0]).max() > thresh  );
			else if(i==0 && j==m)
				edges[0][m] = ( (_data[0][m-1]+_data[1][m]-2*_data[0][m]).max() > thresh  );
			else if(i==n && j==0)
				edges[n][0] = ( (_data[n][1]+_data[n-1][0]-2*_data[n][0]).max() > thresh  );
			else if(i==n && j==m)
				edges[n][m] = ( (_data[n][m-1]+_data[n-1][m]-2*_data[n][m]).max() > thresh  );
			else if(i==0)
				edges[0][j] = ( (_data[0][j-1]+_data[0][j+1]-2*_data[0][j]).max() > thresh  );
			else if(j==0)
				edges[i][0] = ( (_data[i-1][0]+_data[i+1][0]-2*_data[i][0]).max() > thresh  );
			else if(i==n)
				edges[n][j] = ( (_data[n][j-1]+_data[n][j+1]-2*_data[n][j]).max() > thresh  );
			else if(j==m)
				edges[i][m] = ( (_data[i-1][m]+_data[i+1][m]-2*_data[i][m]).max() > thresh  );
			else
				edges[i][j] = ( (_data[i-1][j]+_data[i+1][j]+_data[i][j-1]+_data[i][j+1]-4*_data[i][j]).max() > 2*thresh  );

			//for debugging: set all to 1
			//edges[i][j]=true;
		}
	}
	//widen selection
	std::vector<std::vector<bool> > additional(_data.size(),std::vector<bool>(_data[0].size(),false));
	for(unsigned int i = 1; i <= n-1; ++i)
		for(unsigned int j = 1; j <= m-1; ++j)
			if( edges[i][j+1] or edges[i][j-1] or edges[i+1][j] or edges[i+1][j]
			   or edges[i+1][j+1] or edges[i-1][j-1] or edges[i-1][j+1] or edges[i+1][j-1])
				additional[i][j] = true;


	for(unsigned int i = 1; i <= n-1; ++i)
		for(unsigned int j = 1; j <= m-1; ++j)
			if( additional[i][j] )
				edges[i][j] = true;

	return edges;
}

