#include <iostream>
#include <map>
#include <cassert>
#include <tuple>

template<typename T, T DefaultValue>
class Matrix {
public:
	struct Line {
		Line() = default;

		const T& operator[] (int column) const {
			if (auto it = columnToValue_.find(column); it != columnToValue_.end()) {
				return it->second;
			}
			return DefaultValue;
		}

		T& operator[] (int column) {
			if (auto it = columnToValue_.find(column); it != columnToValue_.end()) {
				return it->second;
			}

			auto p = columnToValue_.emplace(column, DefaultValue);
			return p.first->second;
		}

		std::map<int, T> columnToValue_;
	};

	Matrix()  = default;

	std::size_t size() {
		adjustSize();
		return size_;
	}

	const Line& operator[] (int row) const {
		if (auto it = data_.find(row); it != data_.end()) {
			return it->second;
		}
		return emptyItem_;
	}

	Line& operator[] (int row) {
		isSizeMaybeChanged = true;
		return data_[row];
	}

	
	struct Iterator {
		Iterator(bool isBegin, std::map<int, Line>& data) : data_(data) {
			if (isBegin) {
				mainIter_ = data_.begin();
				if (mainIter_ != data_.end()) {
					auxIter_ = mainIter_->second.columnToValue_.begin();
				}
			} else {
				mainIter_ = data_.end();
			}
		}

		Iterator& operator++ () {
			if (mainIter_ != data_.end() && auxIter_ != mainIter_->second.columnToValue_.end()) {
				++auxIter_;
				if (auxIter_ == mainIter_->second.columnToValue_.end()) {
					++mainIter_;
					if (mainIter_ != data_.end()) {
						auxIter_ = mainIter_->second.columnToValue_.begin();
					}
				}
			}
			return *this;
		}

		std::tuple<int, int, T> operator* () {
			return std::make_tuple(mainIter_->first, auxIter_->first, auxIter_->second);
		}

		bool operator!= (const Iterator& rhs) const {
			return mainIter_ != rhs.mainIter_ &&
				   auxIter_ != rhs.auxIter_;
		}

		std::map<int, Line>& data_;
		typename std::map<int, Line>::iterator mainIter_;
		typename std::map<int, T>::iterator auxIter_;
	};

	Iterator begin() {
		isSizeMaybeChanged = true;
		adjustSize();
		return Iterator(true, data_);
	}

	Iterator end() {
		isSizeMaybeChanged = true;
		adjustSize();
		return Iterator(false, data_);
	}

private:
	void adjustSize() {
		if (isSizeMaybeChanged) {
			size_ = 0;
			for (auto it = data_.begin(); it != data_.end(); ) {
				for (auto itt = it->second.columnToValue_.begin(); itt != it->second.columnToValue_.end(); ) {
					if (itt->second != DefaultValue) {
						size_++;
						++itt;
					} else {
						itt = it->second.columnToValue_.erase(itt);
					}
				}

				if (it->second.columnToValue_.empty()) {
					it = data_.erase(it);
				} else {
					++it;
				}
			}

			isSizeMaybeChanged = false;
		}
	}

	mutable std::map<int, Line> data_; //first == row, second == map <column, value>
	mutable std::size_t size_ = 0;
	mutable bool isSizeMaybeChanged = false;
	const Line emptyItem_;
};

int main() {
	{
		Matrix<int, -1> matrix;
		assert(matrix.size() == 0); // все ячейки свободны
		auto a = matrix[0][0];
		assert(a == -1);
		assert(matrix.size() == 0);
		matrix[100][100] = 314;
		assert(matrix[100][100] == 314);
		assert(matrix.size() == 1);
		matrix[100][100] = -1;
		assert(matrix[100][100] == -1);
		assert(matrix.size() == 0);
	}

	{
		Matrix<int, 0> matrix;
		for (int i = 0; i < 10; i++) {
			matrix[i][i] = i;
		}
		for (int i = 0; i < 10; i++) {
			matrix[i][9 - i] = 9 - i;
		}

		for (int r = 1; r < 9; r++) {
			for (int c = 1; c < 9; c++) {
				std::cout << matrix[r][c] << ' ';
			}
			std::cout << "\b " << std::endl;
		}

		std::cout << matrix.size() << std::endl;

		for (const auto& [x, y, value] : matrix) {
			std::cout << "(" << x << "," << y << "): " << value << std::endl;
		}
	}
}