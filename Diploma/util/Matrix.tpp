#include "Matrix.h"
#include "debug_info.h"


#pragma region Constructors

template <typename T>
Matrix<T>::Matrix(size_t rows, size_t columns) : elements(rows * columns), 
                                                           rows(rows),
														   columns(columns) {}


template <typename T>
Matrix<T>::Matrix(const Matrix<T>& matrix) : elements(matrix.elements), 
                                             rows(matrix.rowsNum), 
                                             columns(matrix.columnsNum) {}


template <typename T>
Matrix<T>::Matrix(Matrix<T>&& rVal) : elements(std::move(rVal.elements))
{
    rows = rVal.rows;
    columns = rVal.columns;

    rVal.rows = 0;
    rVal.columns = 0;
}


template <typename T>
Matrix<T>::Matrix(const Array<T>& array, size_t rows, size_t columns) : elements(array), 
                                                                                  rows(rows), 
                                                                                  columns(columns) 
{
    static_assert(array.size() == rows * columns, ("Matrix with %d rows and %d columns cannot be created from Array of size %d",
                                                   rows, columns, array.size()));
}

#pragma endregion Constructors


#pragma region Matrix parameters

template <typename T>
inline size_t Matrix<T>::rowsNum() const
{
    return rows;
}


template <typename T>
inline size_t Matrix<T>::columnsNum() const
{
    return columns;
}


template <typename T>
inline size_t Matrix<T>::elementsNum() const
{
    return elements.size();
}

#pragma endregion Matrix parameters


#pragma region Access operators

template <typename T>
inline const T& Matrix<T>::operator()(size_t rawIndex) const
{
    assert_message(rawIndex >= 0 && rawIndex < elements.size(), "Matrix raw index is out of bounds");
    return elements(rawIndex);
}


template <typename T>
inline T& Matrix<T>::operator()(size_t rawIndex)
{
    assert_message(rawIndex >= 0 && rawIndex < elements.size(), "Matrix raw index is out of bounds");
    return elements(rawIndex);
}


template <typename T>
inline const T& Matrix<T>::operator()(size_t row, size_t column) const
{
    assert_message(row >= 0 && row < rows, "Matrix row index is out of bounds");
    assert_message(column >= 0 && column < columns, "Matrix column index is out of bounds");
    return elements(row * columns + column);
}


template <typename T>
inline T& Matrix<T>::operator()(size_t row, size_t column)
{
    assert_message(row >= 0 && row < rows, "Matrix row index is out of bounds");
    assert_message(column >= 0 && column < columns, "Matrix column index is out of bounds");
    return elements(row * columns + column);
}

#pragma endregion Access operators


#pragma region Raw elements getters

template <typename T>
inline const Array<T>& Matrix<T>::getRawElements() const
{
    return elements;
}

#pragma endregion Raw elements getters


#pragma region Assignment operators

template <typename T>
inline Matrix<T>& Matrix<T>::operator=(const Matrix<T>& r)
{
    elements = r.elements;
    rows = r.rows;
    columns = r.columns;
}


template <typename T>
inline Matrix<T>& Matrix<T>::operator=(Matrix<T>&& rVal)
{
    elements = std::move(rVal.elements);
    rows = rVal.rows;
    columns = rVal.columns;

    rVal.rows = 0;
    rVal.columns = 0;
}

#pragma endregion Assignment operators


#pragma region Arithmetic operators

template <typename T>
inline Matrix<T> operator+(const Matrix<T>& l, const Matrix<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator + cannot be applied to Matrices of this type");
    assert_message(l.rows == r.rows && l.columns == r.columns, "Operator + cannot be applied to Matrices of different dimentions");
    return Matrix(l.elements + r.elements);
}


template <typename T>
inline Matrix<T> operator-(const Matrix<T>& l, const Matrix<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator - cannot be applied to Matrices of this type");
    assert_message(l.rows == r.rows && l.columns == r.columns, "Operator - cannot be applied to Matrices of different dimentions");
    return Matrix(l.elements - r.elements);
}


template <typename T>
inline Matrix<T> operator*(const Matrix<T>& l, const T& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator * cannot be applied to Matrices of this type");
    return Matrix(l.elements * r);
}


template <typename T>
inline Matrix<T> operator*(const T& l, const Matrix<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator * cannot be applied to Matrices of this type");
    return r * l;
}


template <typename T>
inline Matrix<T> operator/(const Matrix<T>& l, const T& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator / cannot be applied to Matrices of this type");
    return Matrix(l.elements / r);
}

#pragma endregion Arithmetic operators


#pragma region Arithmetic operators with assignment

template <typename T>
inline Matrix<T>& Matrix<T>::operator+=(const Matrix<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator += cannot be applied to Matrices of this type");
    assert_message(rows == r.rows && columns == r.columns, "Operator += cannot be applied to Matrices of different dimentions");

    elements += r.elements;

    return *this;
}


template <typename T>
inline Matrix<T>& Matrix<T>::operator-=(const Matrix<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator -= cannot be applied to Matrices of this type");
    assert_message(rows == r.rows && columns == r.columns, "Operator -= cannot be applied to Matrices of different dimentions");

    elements -= r.elements;

    return *this;
}


template <typename T>
inline Matrix<T>& Matrix<T>::operator*=(const T& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator *= cannot be applied to Matrices of this type");

    elements *= r;

    return *this;
}


template <typename T>
inline Matrix<T>& Matrix<T>::operator/=(const T& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator /= cannot be applied to Matrices of this type");

    elements /= r;

    return *this;
}

#pragma endregion Arithmetic operators with assignment


#pragma region Swap methods

template <typename T>
inline void Matrix<T>::swap(Matrix<T>& other)
{
    static_assert(is_swappable_ext<T>::value, "Swap cannot be applied to Matrix of this type");
    std::swap(elements, other.elements);
    std::swap(rows, other.rows);
    std::swap(columns, other.columns);
}


template <typename T>
inline void swap(Matrix<T>& first, Matrix<T>& second)
{
    first.swap(second);
}

#pragma endregion Swap methods


#pragma region Norms methods

template <typename T>
T norm(const Matrix<T>& a, const Matrix<T>& b)
{
    static_assert(is_arithmetic_ext<T>::value, "Norm cannot be calculated for Matrices of this type");
    assert_message(a.rowsNum() == b.rowsNum() && a.columnsNum() == b.columnsNum(), 
                   "Norm cannot be calculated for Matrices of different dimentions");
    return norm(a.elements, b.elements);
}

#pragma endregion Norms methods
