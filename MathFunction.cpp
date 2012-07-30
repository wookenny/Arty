#include "MathFunction.h"

int main(int args, char **argv ){

	std::string term{"1+2*3-x"};
	double x = 1.4;
	if(args>1)
		term = std::string(argv[1]);
	if(args>2)
		x = boost::lexical_cast<double>(argv[2]);

	MathFunction<double> f{term};

	std::cout<<f(x)<<std::endl;



	f.test();
}



