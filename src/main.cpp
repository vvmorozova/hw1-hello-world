#include "matrix.h"

#include <memory>
#include <iostream>
#include <string>

int main(int, char **) {
	
	Matrix<int, -1> mat;

	mat[10][2] = 9;
	//mat[2][3] = 4;

	//mat.print(11, 11);
	return 0;
} 


