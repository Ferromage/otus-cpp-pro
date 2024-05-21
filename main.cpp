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

	struct Iterator {
		Iterator(bool isBegin, std::map<int, Line>& data) : data_(data) {
			if (isBegin) {
				rowIter_ = data_.begin();
				if (rowIter_ != data_.end()) {
					columnIter_ = rowIter_->second.columnToValue_.begin();
				}
			} else {
				rowIter_ = data_.end();
			}
		}

		Iterator& operator++ () {
			if (rowIter_ != data_.end() && columnIter_ != rowIter_->second.columnToValue_.end()) {
				++columnIter_;
				if (columnIter_ == rowIter_->second.columnToValue_.end()) {
					++rowIter_;
					if (rowIter_ != data_.end()) {
						columnIter_ = rowIter_->second.columnToValue_.begin();
					}
				}
			}
			return *this;
		}

		std::tuple<int, int, T> operator* () {
			return std::make_tuple(rowIter_->first, columnIter_->first, columnIter_->second);
		}

		bool operator!= (const Iterator& rhs) const {
			return rowIter_ != rhs.rowIter_ &&
				   columnIter_ != rhs.columnIter_;
		}

		std::map<int, Line>& data_;
		typename std::map<int, Line>::iterator rowIter_;
		typename std::map<int, T>::iterator columnIter_;
	};

	Matrix() = default;

	std::size_t size() {
		calcSize();
		return size_;
	}

	const Line& operator[] (int row) const {
		if (auto it = data_.find(row); it != data_.end()) {
			return it->second;
		}
		return emptyLine_;
	}

	Line& operator[] (int row) {
		return data_[row];
	}

	Iterator begin() {
		return Iterator(true, data_);
	}

	Iterator end() {
		return Iterator(false, data_);
	}

private:
	void calcSize() {
		size_ = 0;
		for (auto it = data_.begin(); it != data_.end(); ++it) {
			for (auto itt = it->second.columnToValue_.begin(); itt != it->second.columnToValue_.end(); ++itt) {
				size_++;
			}
		}
	}

	template<typename U, U> friend class MatrixProxy;

	mutable std::map<int, Line> data_; //row index to line
	std::size_t size_ = 0;
	const Line emptyLine_;
};


template<typename T, T DefaultValue>
class MatrixProxy {
public:
	MatrixProxy(Matrix<T, DefaultValue>& matrix) : matrix_(matrix) {}

	std::size_t size() {
		adjustSize();
		return matrix_.size();
	}

	const typename Matrix<T, DefaultValue>::Line& operator[] (int row) const {
		return matrix_.operator[](row);
	}

	typename Matrix<T, DefaultValue>::Line& operator[] (int row) {
		isSizeMaybeChanged = true;
		return matrix_.operator[](row);
	}

	typename Matrix<T, DefaultValue>::Iterator begin() {
		adjustSize();
		return matrix_.begin();
	}

	typename Matrix<T, DefaultValue>::Iterator end() {
		adjustSize();
		return matrix_.end();
	}

private:
	void adjustSize() {
		if (isSizeMaybeChanged) {
			for (auto it = matrix_.data_.begin(); it != matrix_.data_.end(); ) {
				for (auto itt = it->second.columnToValue_.begin(); itt != it->second.columnToValue_.end(); ) {
					if (itt->second != DefaultValue) {
						++itt;
					} else {
						itt = it->second.columnToValue_.erase(itt);
					}
				}

				if (it->second.columnToValue_.empty()) {
					it = matrix_.data_.erase(it);
				} else {
					++it;
				}
			}

			isSizeMaybeChanged = false;
		}
	}

	Matrix<T, DefaultValue>& matrix_;
	bool isSizeMaybeChanged = false;
};


int main() {
	{
		Matrix<int, -1> matrix;
		assert(matrix.size() == 0);
		auto a = matrix[0][0];
		assert(a == -1);
		assert(matrix.size() == 1);
		matrix[100][100] = 314;
		assert(matrix[100][100] == 314);
		assert(matrix.size() == 2);
		matrix[100][100] = -1;
		assert(matrix[100][100] == -1);
		assert(matrix.size() == 2);
	}

	{
		Matrix<int, -2> matrix;
		MatrixProxy proxy(matrix);
		assert(proxy.size() == 0); // все ячейки свободны
		auto a = proxy[0][0];
		assert(a == -2);
		assert(proxy.size() == 0);
		proxy[100][100] = 314;
		assert(proxy[100][100] == 314);
		assert(proxy.size() == 1);
		proxy[100][100] = -2;
		assert(proxy[100][100] == -2);
		assert(proxy.size() == 0);
	}
	
	{
		Matrix<int, 0> matrix;
		MatrixProxy proxy(matrix);

		for (int i = 0; i < 10; i++) {
			proxy[i][i] = i;
		}
		for (int i = 0; i < 10; i++) {
			proxy[i][9 - i] = 9 - i;
		}

		for (int r = 1; r < 9; r++) {
			for (int c = 1; c < 9; c++) {
				std::cout << proxy[r][c] << ' ';
			}
			std::cout << "\b " << std::endl;
		}

		std::cout << proxy.size() << std::endl;

		for (const auto& [x, y, value] : proxy) {
			std::cout << "(" << x << "," << y << "): " << value << std::endl;
		}
	}
}