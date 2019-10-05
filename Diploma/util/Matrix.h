#ifndef DIPLOMA_MATRIX_H
#define DIPLOMA_MATRIX_H

#include "Array.h"



template <typename T>
class Matrix
{
public:
	Matrix(arr_size_t rowsNum, arr_size_t columnsNum);

    Matrix(const Matrix<T>& matrix);

    Matrix(Matrix<T>&& rVal);

	
    arr_size_t rowsNum() const;

    arr_size_t columnsNum() const;

    arr_size_t elementsNum() const;


    const T& operator()(arr_size_t rawIndex) const;

    T& operator()(arr_size_t rawIndex);

    const T& operator()(arr_size_t row, arr_size_t column) const;

    T& operator()(arr_size_t row, arr_size_t column);

    const T& operator()(const Vector2<arr_size_t>& indices) const;

    T& operator()(const Vector2<arr_size_t>& indices);


    Matrix<T>& operator=(const Matrix<T>& r);

    Matrix<T>& operator=(Matrix<T>&& rVal);


    Matrix<T>& operator+=(const Matrix<T>& r);

    Matrix<T>& operator-=(const Matrix<T>& r);

    template <typename K>
    Matrix<T>& operator*=(const K& r);

    template <typename K>
    Matrix<T>& operator/=(const K& r);


    void swap(Matrix<T>& other);



    friend T norm<T>(const Matrix<T>& a, const Matrix<T>& b);

private:
	Array<T> mElements;
	arr_size_t mRowsNum;
	arr_size_t mColumnsNum;


    Matrix(const Array<T>& elements, arr_size_t rowsNum, arr_size_t columnsNum);
};


template <typename T>
Matrix<T> operator+(const Matrix<T>&l, const Matrix<T>& r);

template <typename T>
Matrix<T> operator-(const Matrix<T>& l, const Matrix<T>& r);

template <typename T, typename K>
Matrix<T> operator*(const Matrix<T>& l, const K& r);

template <typename T, typename K>
Matrix<T> operator*(const K& l, const Matrix<T>& r);

template <typename T, typename K>
Matrix<T> operator/(const Matrix<T>& l, const K& r);


template <typename T>
void swap(Matrix<T>& first, Matrix<T>& second);



#include "Matrix.tpp"

#endif