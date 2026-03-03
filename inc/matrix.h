#include "unordered_map"
#include "vector"
#include <iostream>

template <typename T, T defValue>
class Matrix {
private:
	
	std::unordered_map<int, std::unordered_map<int, T>> m_mapData;

	int m_size;
	T m_defVal;
	int m_occCells;
	
public:
	//int size();

	Matrix() : m_defVal(defValue) {};

	class RowProxy {
	private:
		std::unordered_map<int, T>&row;
		T &defVal;
	public:
		RowProxy(std::unordered_map<int, T> &r, T &m_defVal) : row(r), defVal(m_defVal) {}

		T& operator[](std::size_t col) {
			return row.find(col) == row.end() ? defVal : row.at(col);
		}
	};

	RowProxy operator[](std::size_t row) {
		return RowProxy(m_mapData.at(row), m_defVal);
	}

	const RowProxy operator[](std::size_t row) const {
		return RowProxy(const_cast<std::unordered_map<int, T>&>(m_mapData.at(row)), m_defVal);
	}

	void print(int row, int col)
	{
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				if (m_mapData.find(i) != m_mapData.end()) {
					auto elem = m_mapData.find(i)->second;
					if (elem.find(j) != elem.end()) {
						std::cout << j << " ";
					}
					else {
						std::cout << m_defVal;
					}
				}
				else {

					std::cout << m_defVal;
				}
			}
		}
	}

	//void printOcc();

	//operator=
};