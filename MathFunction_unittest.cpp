//Compiling with this: 

//g++ MathFunction_unittest.cpp -std=c++0x -lgtest_main -lgtest -lpthread 

#include "MathFunction.h"
#include <gtest/gtest.h>

// Tests factorial of negative numbers.
TEST(FunctionTest/*Testcase Name*/, Double/*Test name*/) {

  //function setup
  auto func = [] (double x) {return 1+2*3-x;};
  std::string term{"1+2*3-x"};
  MathFunction<double> f{term};

  double values[] = {-5.1,0,2.4,1,56,-73.42,2.34,4323.2,1.337,-21.231};
  for ( auto x : values )
  	EXPECT_DOUBLE_EQ(func(x), f(x));
} 

TEST(FunctionTest, Integer) {
  
  auto func = [] (int x) {return 1+3*(int)x;};
  std::string term{"1+3*x"};
  MathFunction<int> f{term};
  double values[] = {-5.1,0,2.4,1,56,-7342,234,43232,1337,-21231};

  for ( auto x : values )
  	EXPECT_EQ(func(x), f(x));
}

TEST(FunctionTest, Power) {
  
  auto func = [] (double x) {return pow(3,x);};
  std::string term{"3^x"};
  MathFunction<double> f{term};
  double values[] = {-5.1,0,2.4,1,56,-7342,234,43232,1337,-21231};

  for ( auto x : values )
  	EXPECT_DOUBLE_EQ(func(x), f(x));
}

TEST(FunctionTest, Brackets) {
  
  auto func = [] (double x) {return (x+x)*(x-3);};
  MathFunction<double> f{"(x+x)*(x-3)"};
  double values[] = {-5.1,0,2.4,1,56,-7342,234,43232,1337,-21231};

  for ( auto x : values )
  	EXPECT_DOUBLE_EQ(func(x), f(x));
}

TEST(FunctionTest, SpecialTests) {
    
  MathFunction<double> f2{"x - -3"};
  EXPECT_DOUBLE_EQ(f2(0), 3);
}
