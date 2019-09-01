#include "Matrix.h"
#include "debug_info.h"


#pragma region Constructors

template <typename T>
Matrix<T>::Matrix(arr_size_t rowsNum, arr_size_t columnsNum) : mElements(rowsNum * columnsNum), 
                                                               mRowsNum(rowsNum),
													           mColumnsNum(columnsNum) {}


template <typename T>
Matrix<T>::Matrix(const Matrix<T>& matrix) : mElements(matrix.mElements), 
                                             mRowsNum(matrix.mRowsNum), 
                                             mColumnsNum(matrix.mcolumnsNum) {}


template <typename T>
Matrix<T>::Matrix(Matrix<T>&& rVal) : mElements(std::move(rVal.mElements))
{
    mRowsNum = rVal.mRowsNum;
    mColumnsNum = rVal.mColumnsNum;

    rVal.mRowsNum = 0;
    rVal.mColumnsNum = 0;
}


template <typename T>
Matrix<T>::Matrix(const Array<T>& array, arr_size_t rowsNum, arr_size_t columnsNum) : mElements(array), 
                                                                                      mRowsNum(rowsNum), 
                                                                                      mColumnsNum(columnsNum) 
{
    static_assert(array.size() == rowsNum * columnsNum, ("Matrix with %d mRows and %d columns cannot be created from Array of size %d",
                                                         mRowsNum, mColumnsNum, array.size()));
}

#pragma endregion


#pragma region Matrix parameters

template <typename T>
inline arr_size_t Matrix<T>::rowsNum() const
{
    return mRowsNum;
}


template <typename T>
inline arr_size_t Matrix<T>::columnsNum() const
{
    return mColumnsNum;
}


template <typename T>
inline arr_size_t Matrix<T>::elementsNum() const
{
    return mElements.size();
}

#pragma endregion


#pragma region Access operators

template <typename T>
inline const T& Matrix<T>::operator()(arr_size_t rawIndex) const
{
    assert_message(rawIndex >= 0 && rawIndex < mElements.size(), "Matrix raw index is out of bounds");
    return mElements(rawIndex);
}


template <typename T>
inline T& Matrix<T>::operator()(arr_size_t rawIndex)
{
    assert_message(rawIndex >= 0 && rawIndex < mElements.size(), "Matrix raw index is out of bounds");
    return mElements(rawIndex);
}


template <typename T>
inline const T& Matrix<T>::operator()(arr_size_t row, arr_size_t column) const
{
    assert_message(row >= 0 && row < mRowsNum, "Matrix row index is out of bounds");
    assert_message(column >= 0 && column < mColumnsNum, "Matrix column index is out of bounds");
    return mElements(row * mColumnsNum + column);
}


template <typename T>
inline T& Matrix<T>::operator()(arr_size_t row, arr_size_t column)
{
    assert_message(row >= 0 && row < mRowsNum, "Matrix row index is out of bounds");
    assert_message(column >= 0 && column < mColumnsNum, "Matrix column index is out of bounds");
    return mElements(row * mColumnsNum + column);
}

#pragma endregion


#pragma region Assignment operators

template <typename T>
inline Matrix<T>& Matrix<T>::operator=(const Matrix<T>& r)
{
    mElements = r.mElements;
    mRowsNum = r.mRowsNum;
    mColumnsNum = r.mColumnsNum;
}


template <typename T>
inline Matrix<T>& Matrix<T>::operator=(Matrix<T>&& rVal)
{
    mElements = std::move(rVal.mElements);
    mRowsNum = rVal.mRowsNum;
    mColumnsNum = rVal.mColumnsNum;

    rVal.mRowsNum = 0;
    rVal.mColumnsNum = 0;
}

#pragma endregion


#pragma region Arithmetic operators

template <typename T>
inline Matrix<T> operator+(const Matrix<T>& l, const Matrix<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator + cannot be applied to Matrices of this type");
    assert_message(l.mRowsNum == r.mRowsNum && l.mColumnsNum == r.mColumnsNum, 
                   "Operator + cannot be applied to Matrices of different dimentions");
    return Matrix(l.mElements + r.mElements);
}


template <typename T>
inline Matrix<T> operator-(const Matrix<T>& l, const Matrix<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator - cannot be applied to Matrices of this type");
    assert_message(l.mRowsNum == r.mRowsNum && l.mColumnsNum == r.mColumnsNum, 
                   "Operator - cannot be applied to Matrices of different dimentions");
    return Matrix(l.mElements - r.mElements);
}


template <typename T>
inline Matrix<T> operator*(const Matrix<T>& l, const T& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator * cannot be applied to Matrices of this type");
    return Matrix(l.mElements * r);
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
    return Matrix(l.mElements / r);
}

#pragma endregion


#pragma region Arithmetic operators with assignment

template <typename T>
inline Matrix<T>& Matrix<T>::operator+=(const Matrix<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator += cannot be applied to Matrices of this type");
    assert_message(mRowsNum == r.mRowsNum && mColumnsNum == r.mColumnsNum, 
                   "Operator += cannot be applied to Matrices of different dimentions");

    mElements += r.mElements;

    return *this;
}


template <typename T>
inline Matrix<T>& Matrix<T>::operator-=(const Matrix<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator -= cannot be applied to Matrices of this type");
    assert_message(mRowsNum == r.mRowsNum && mColumnsNum == r.mColumnsNum, 
                   "Operator -= cannot be applied to Matrices of different dimentions");

    mElements -= r.mElements;

    return *this;
}


template <typename T>
inline Matrix<T>& Matrix<T>::operator*=(const T& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator *= cannot be applied to Matrices of this type");

    mElements *= r;

    return *this;
}


template <typename T>
inline Matrix<T>& Matrix<T>::operator/=(const T& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator /= cannot be applied to Matrices of this type");

    mElements /= r;

    return *this;
}

#pragma endregion


#pragma region Swap methods

template <typename T>
inline void Matrix<T>::swap(Matrix<T>& other)
{
    static_assert(is_swappable_ext<T>::value, "Swap cannot be applied to Matrix of this type");
    std::swap(mElements, other.mElements);
    std::swap(mRowsNum, other.mRowsNum);
    std::swap(mColumnsNum, other.mColumnsNum);
}


template <typename T>
inline void swap(Matrix<T>& first, Matrix<T>& second)
{
    first.swap(second);
}

#pragma endregion


#pragma region Norms methods

template <typename T>
T norm(const Matrix<T>& a, const Matrix<T>& b)
{
    static_assert(is_arithmetic_ext<T>::value, "Norm cannot be calculated for Matrices of this type");
    assert_message(a.rowsNum() == b.rowsNum() && a.columnsNum() == b.columnsNum(), 
                   "Norm cannot be calculated for Matrices of different dimentions");
    return norm(a.mElements, b.mElements);
}

#pragma endregion
