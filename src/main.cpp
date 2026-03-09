#include "matrix.h"

#include <memory>
#include <iostream>
#include <string>

int main(int, char **) {
	
	/*Matrix<int, 0> mat;

	for (int i = 0; i < 10; i++) {
		mat[i][i] = i;
		mat[i][9 - i] = 9 - i;
	}

	std::cout << "Matrix [1,1] to [8,8]" << std::endl;
	mat.print(1, 8);
	std::cout << std::endl;

	std::cout << mat.getOcc() << " occupied cells" << std::endl;
	std::cout << std::endl;

	std::cout << "Occupied cells" << std::endl;
	for (auto cell : mat) {
		std::cout << "mat[" << cell.first.first <<  "][" << cell.first.second << "] = "
		<< cell.second << std::endl;
	}*/

	std::unique_ptr<Matrix<int, 0>> m = std::make_unique<Matrix<int, 0>>();
	(*m)[0][0] = 2;
	auto proxy = (*m)[0];
	m.reset();
	std::cout << proxy[0] << std::endl;
	return 0;
} 


