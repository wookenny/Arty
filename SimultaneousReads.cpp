#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <tuple>

//compile this via
//g++ SimultaneousReads.cpp  -std=c++0x -pthread -O3


//this is the size of the used data => increase this to get a greater running time
static uint DATA_SIZE = 4096;


typedef unsigned int uint;
typedef std::vector<float> row;
typedef std::vector<row> matrix;



int getNumCores(){ return sysconf( _SC_NPROCESSORS_ONLN ); }


//This class stores the small pieces of work. Every thread can come and ask for
//a new piece of work. During this the pool is modified(i.e. the current counter
// id increased). Do garantee a valid state during the whole time, the private
// mutex SHOULD be used to lock the pool.
class ThreadDataPool{
	private:
		std::vector<std::tuple<int,int,int,int> > _vec;
		uint _curr;
		std::mutex _m;

	public:

		ThreadDataPool() = delete;
		ThreadDataPool(const ThreadDataPool&) = delete;
		ThreadDataPool(int xmin, int xmax, int xinc, int ymin, int ymax, int yinc){
			for(int x = xmin; x < xmax; x+=xinc)
				for(int y = ymin; y < ymax; y+=yinc){
					_vec.push_back( std::make_tuple(x,std::min(x+xinc,xmax),
													y,std::min(y+yinc,ymax)) );
				}
			_curr = 0;
		}

		uint size(){ return _vec.size();}
		std::mutex& mutex(){ return _m;}
		bool atEnd() const{ return _curr >= _vec.size();}
		std::tuple<int,int,int,int> next(){ _curr++; return _vec.at(_curr-1); }

};

//here is the functor which ueses the pool to get new datan and work on it
//it will be used as a single thread
class CalcFunctorWithPool{
	private:
		ThreadDataPool &_pool;
		matrix &m;
		matrix &result;

		void calc(int i, int j){
			float r = 0;
			for(uint l = 0; l<DATA_SIZE;++l)
				r += m[i][l]*m[j][l];
			result[i][j] = r;
		}

	public:
		CalcFunctorWithPool() = delete;
		CalcFunctorWithPool(ThreadDataPool &p, matrix &mat, matrix &r):
			_pool(p),m(mat),result(r){}
		//here comes the mutex aware stuff
		void operator()(){
			std::tuple<int,int,int,int> t;
			while( true ){
				{//scope for mutex
					std::lock_guard<std::mutex> lk(_pool.mutex());
					if( _pool.atEnd() )
						break;
					t = _pool.next();
				}
				for(int i = std::get<0>(t); i<std::get<1>(t); ++i)
					for(int j = std::get<2>(t); j<std::get<3>(t); ++j)
						calc(i,j);
			}
		}
};


int main(int argc, char **argv){
	srand(0);
	uint NUM_THREADS = 1;
	if(argc > 1)
		NUM_THREADS = atoi(argv[1]);
	else
		NUM_THREADS = getNumCores();

	std::cout<< "cores found: "<<  getNumCores() <<
	"\tusing: "<<NUM_THREADS<<std::endl;

	matrix data;//reading on this
	for(uint i = 0; i<DATA_SIZE; ++i){
		row r;
		data.push_back(r);
		for(uint j = 0; j<DATA_SIZE; ++j){
			data.at(i).push_back(rand());
		}
	}
	std::cout<<"data created"<<std::endl;

	//creating resulting data
	matrix result;
	for(uint i = 0; i < DATA_SIZE ; ++i)
		result.push_back(row(DATA_SIZE,0.));
	std::cout << "resulting matrix created" << std::endl;

	//main calculation
	typedef std::chrono::high_resolution_clock Clock;
	typedef std::chrono::duration<double> sec;
	std::vector<std::thread> threads;
	Clock::time_point start;
	Clock::time_point end;

	//threads with pool:
	start = Clock::now();
	ThreadDataPool pool(0, DATA_SIZE, 10/*xinc*/, 0, DATA_SIZE, 10/*yinc*/);
	threads.clear() ;
	for(uint i =0; i<NUM_THREADS; ++i){
		threads.push_back(
			std::thread{CalcFunctorWithPool(pool,data,result)}
		);
	}
	for(uint i=0; i<threads.size(); ++i)
		threads[i].join();
	end = Clock::now();
	std::cout << sec(end-start).count() << " seconds when running on "<<NUM_THREADS <<" threads " <<"\n";
	std::cout << "check: " << result[DATA_SIZE-1][DATA_SIZE-1] << std::endl;

}

