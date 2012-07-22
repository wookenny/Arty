//Compiling with this: 

//g++ MathFunction_unittest.cpp -std=c++0x -lgtest_main -lgtest -lpthread 


// This sample shows how to write a simple unit test for a function,
// using Google C++ testing framework.
//
// Writing a unit test using Google C++ testing framework is easy as 1-2-3:


// Step 1. Include necessary header files such that the stuff your
// test logic needs is declared.
//
// Don't forget gtest.h, which declares the testing framework.

#include "MathFunction.h"
#include <gtest/gtest.h>

// Step 2. Use the TEST macro to define your tests.
//
// TEST has two parameters: the test case name and the test name.
// After using the macro, you should define your test logic between a
// pair of braces.  You can use a bunch of macros to indicate the
// success or failure of a test.  EXPECT_TRUE and EXPECT_EQ are
// examples of such macros.  For a complete list, see gtest.h.
//
// <TechnicalDetails>
//
// In Google Test, tests are grouped into test cases.  This is how we
// keep test code organized.  You should put logically related tests
// into the same test case.
//
// The test case name and the test name should both be valid C++
// identifiers.  And you should not use underscore (_) in the names.
//
// Google Test guarantees that each test you define is run exactly
// once, but it makes no guarantee on the order the tests are
// executed.  Therefore, you should write your tests in such a way
// that their results don't depend on their order.
//
// </TechnicalDetails>

 // <TechnicalDetails>
  //
  // EXPECT_EQ(expected, actual) is the same as
  //
  //   EXPECT_TRUE((expected) == (actual))
  //
  // except that it will print both the expected value and the actual
  // value when the assertion fails.  This is very helpful for
  // debugging.  Therefore in this case EXPECT_EQ is preferred.
  //
  // On the other hand, EXPECT_TRUE accepts any Boolean expression,
  // and is thus more general.
  //
  // </TechnicalDetails>



// Tests factorial of negative numbers.
TEST(FunctionTest, Double) {
  // This test is named "Negative", and belongs to the "FactorialTest"
  // test case.
  std::string term{"1+2*3-x"};
  MathFunction<double> f{term};
  EXPECT_DOUBLE_EQ(12.1, f(-5.1));
  EXPECT_DOUBLE_EQ(7, f(0));
  EXPECT_DOUBLE_EQ(4.6, f(2.4));
}

TEST(FunctionTest, Integer) {
  // This test is named "Negative", and belongs to the "FactorialTest"
  // test case.
  std::string term{"1+3*x"};
  MathFunction<int> f{term};
  EXPECT_DOUBLE_EQ(-14, f(-5.1));
  EXPECT_DOUBLE_EQ(1, f(0));
  EXPECT_DOUBLE_EQ(7, f(2.4));
  
}



// Step 3. Call RUN_ALL_TESTS() in main().
//
// We do this by linking in src/gtest_main.cc file, which consists of
// a main() function which calls RUN_ALL_TESTS() for us.
//
// This runs all the tests you've defined, prints the result, and
// returns 0 if successful, or 1 otherwise.
//
// Did you notice that we didn't register the tests?  The
// RUN_ALL_TESTS() macro magically knows about all the tests we
// defined.  Isn't this convenient?

