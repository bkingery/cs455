
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cml/cml.h>
#include <vector>
using namespace cml;
using namespace std;

//using std::cout;
//using std::endl;
//using std::sqrt;

void printArray(float* array, int size)
{
  for (int i=0; i<size; i++)
	cout << array[i] << " ";
  cout << endl;
}

void test1()
{
  cout <<"\n********** Test1: Read write to simple arrays **********" << endl;
  typedef cml::vector< float, external<4> > vector4;
  typedef cml::matrix<float, external<4,4>, col_basis, col_major> matrix4;
  
  float a[16] = {1, 0, 0, 0, 2, 5, 0, 0, 3, 6, 8, 0, 4, 7, 9, 0};
  cout << "a = ";
  printArray(a, 16);
  matrix4 A(a);
  cout << "A = \n" << A << endl;
  cout << "A back to array: ";
  printArray(A.data(), 16);
  
  float v[16] = {1,2,3,4};
  cout << "\nv = ";
  printArray(v, 4);
  vector4 V(v);
  cout << "V = " << V << endl;
  cout << "V back to array: ";
  printArray(V.data(), 4);
}

void test2()
{
  cout <<"\n********** Test2: Multilpy vectors by single numbers **********" << endl;
  vector4f v(1,2,3,4);
  cout << "v = " << v << endl;
  v= v*2;
  cout << "v*2 = " << v << endl;
}

void test3()
{
  cout <<"\n********** Test3: Adding vectors **********" << endl;
  vector4f v(1,2,3,4);
  vector4f u(5,6,7,8);
  cout << "v = " << v << endl;
  cout << "u = " << u << endl;
  cout << "v+u = " << v+u << endl;
}

void test4()
{
  cout <<"\n********** Test4: Subtracting vectors **********" << endl;
  vector4f v(1,2,3,4);
  vector4f u(5,6,7,8);
  cout << "v = " << v << endl;
  cout << "u = " << u << endl;
  cout << "v+u = " << v-u << endl;
}

void test5()
{
  cout <<"\n********** Test5: Multiply matrices **********" << endl;
  matrix44f_c A(1,2,3,4,
				0,5,6,7,
				0,0,8,9,
				0,0,0,0);
  
  matrix44f_c B(1,0,1,0,
				0,2,0,2,
				0,0,3,0,
				0,0,0,4);
  
  cout << "A = \n" << A << endl;
  cout << "B = \n" << B << endl;
  cout << "A*B = \n" << A*B << endl;
}

void test6()
{
  cout <<"\n********** Test6: Multiply vectors by matrices **********" << endl;
  matrix44f_c A(1,2,3,4,
				0,5,6,7,
				0,0,8,9,
				0,0,0,0);
  
  vector4f v(2,3,5,7);
  
  cout << "A = \n" << A << endl;
  cout << "v = " << v << endl;
  cout << "A*v = " << A*v << endl;
}

void test7()
{
  cout <<"\n********** Test7: Inner(dot) product of 2 vectors **********" << endl;
  vector4f v(1,2,3,4);
  vector4f u(5,6,7,8);
  cout << "v = " << v << endl;
  cout << "u = " << u << endl;
  cout << "dot product of v and u = " << dot(v,u) << endl;
}


void test8()
{
  cout <<"\n********** Test8: 3D cross product of 2 vectors **********" << endl;
  vector4f v(1,2,3,4);
  vector4f u(5,6,7,8);
  cout << "v = " << v << endl;
  cout << "u = " << u << endl;
  cout << "cross product of v and u = " << cross(vector3f(v.data()),vector3f(u.data())) << endl;
}

void test9()
{
  cout <<"\n********** Test9: Matrix inversion **********" << endl;
  matrix44f_c A(2,3,1,5,
				1,0,3,1,
				0,2,-3,2,
				0,2,3,1);
  matrix44f_c B = A;
  B.inverse();
  cout << "A = \n" << A << endl;
  cout << "Inverse of A = B = \n" << B << endl;
  cout << "A*B = \n" << A*B << endl;
}

void test10()
{
  cout <<"\n********** Test10: Push and pop matricies on a stack **********" << endl;
  std::vector<matrix44f> stack;
  cout << "Initial stack size: " << stack.size() << endl;
  cout << "Pushing...";
  for (int i=0; i<32; i++)
  {
	matrix44f_c A(i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i);
	stack.push_back(A);
  }
  cout << "Stack size after push: " << stack.size() << endl;
  cout << "Top should contain 31:\n" << stack.back() << endl;
  cout << "Popping...";
  for (int i=0; i<31; i++)
  {
	stack.pop_back();
  }
  matrix44f_c last = stack.back();
  stack.pop_back();
  cout << "Last should contain 0:\n" << last << endl;
  cout << "Stack should be empty, size: " << stack.size() << endl;
}

int main ( int argc, char** argv )   
{
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();
  test10();
  return 0;
}

