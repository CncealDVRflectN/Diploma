#ifndef DIPLOMA_MATRIX_H
#define DIPLOMA_MATRIX_H

#include "Array.h"



template <typename T>
class Matrix
{
public:
	Matrix(size_t rows, size_t columns);

    Matrix(const Matrix<T>& matrix);

    Matrix(Matrix<T>&& rVal);

	
    size_t rowsNum() const;

    size_t columnsNum() const;

    size_t elementsNum() const;


    const T& operator()(size_t rawIndex) const;

    T& operator()(size_t rawIndex);

    const T& operator()(size_t row, size_t column) const;

    T& operator()(size_t row, size_t column);


    const Array<T>& getRawElements() const;


    Matrix<T>& operator=(const Matrix<T>& r);

    Matrix<T>& operator=(Matrix<T>&& rVal);


    Matrix<T>& operator+=(const Matrix<T>& r);

    Matrix<T>& operator-=(const Matrix<T>& r);

    Matrix<T>& operator*=(const T& r);

    Matrix<T>& operator/=(const T& r);


    void swap(Matrix<T>& other);



    friend T norm<T>(const Matrix<T>& a, const Matrix<T>& b);

private:
	Array<T> elements;
	const size_t rows;
	const size_t columns;


    Matrix(const Array<T>& elements, size_t rows, size_t columns);
};


template <typename T>
Matrix<T> operator+(const Matrix<T>&l, const Matrix<T>& r);

template <typename T>
Matrix<T> operator-(const Matrix<T>& l, const Matrix<T>& r);

template <typename T>
Matrix<T> operator*(const Matrix<T>& l, const T& r);

template <typename T>
Matrix<T> operator*(const T& l, const Matrix<T>& r);

template <typename T>
Matrix<T> operator/(const Matrix<T>& l, const T& r);


template <typename T>
void swap(Matrix<T>& first, Matrix<T>& second);



#include "Matrix.tpp"

#endif