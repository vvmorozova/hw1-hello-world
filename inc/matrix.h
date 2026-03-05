#include <unordered_map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <bit>
#include <bitset>

template <typename T, T defValue>
class Matrix {
private:
	
	struct pairHash {
		std::size_t operator() (const std::pair<int,int>& p) const {
			return std::hash<int>{}(p.first) ^ std::hash<int>{}(p.second);
		}
	};
	std::unordered_map<std::pair<int,int>, T, pairHash> m_mapData;

	int m_size;
	T m_defVal;
	
public:
	Matrix() : m_defVal(defValue) {};

	class CellProxy {
	private:
		Matrix &m_mat;
		int m_row, m_col;
	public:
		CellProxy(Matrix &mat, int row, int col) : m_mat(mat), m_row(row), m_col(col) {}

		CellProxy& operator=(const T&val) {
			if (val == defValue) {
				m_mat.m_mapData.erase({m_row, m_col});
			}
			else {
				m_mat.m_mapData[{m_row, m_col}] = val;
			}
			return *this;
		}

		operator T() const {
			auto it = m_mat.m_mapData.find({m_row, m_col});
			return it != m_mat.m_mapData.end() ? it->second : defValue;
		}
	};

	class RowProxy {
	private:
		int m_row;
		Matrix &m_mat;

	public:
		RowProxy(Matrix &m, int row) : m_row(row), m_mat(m) {}

		CellProxy operator[](int col) {
			return CellProxy(m_mat, m_row, col);
		}
	};

	RowProxy operator[](std::size_t row) {
		return RowProxy(*this, row);
	}

	void print(int low, int high)
	{
		for (int i = low; i <= high; i++) {
			for (int j = low; j <= high; j++) {
				std::cout << std::setw(3) << (*this)[i][j] << " ";
			}
			std::cout << std::endl;
		}
	}

	int getOcc() const
	{
		return m_mapData.size();
	}

	auto begin()
	{
		return m_mapData.begin();
	}

	auto end()
	{
		return m_mapData.end();
	}
};