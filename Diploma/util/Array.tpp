#include <algorithm>

#include "Array.h"
#include "type_traits_ext.h"
#include "debug_info.h"


#pragma region Type traits extension

template <typename T>
struct is_arithmetic_ext<Array<T>>
{
    static const bool value = std::is_arithmetic<T>::value;
};


template <typename T>
struct is_swappable_ext<Array<T>>
{
    static const bool value = std::is_swappable<T>::value;
};

#pragma endregion


#pragma region Constructors

template <typename T>
Array<T>::Array(arr_size_t size) : mElements(size), mSize(size) 
{
    assert_message(size > 0, "Incorrect Array size");
}


template <typename T>
Array<T>::Array(const std::valarray<T>& valArray) : mElements(valArray), mSize(valArray.size()) {}


template <typename T>
Array<T>::Array(const std::vector<T>& vector) : mElements(vector.data(), vector.size()), mSize(vector.size()) {}


template <typename T>
Array<T>::Array(std::initializer_list<T> initList) : mElements(initList), mSize(initList.size()) {}


template <typename T>
Array<T>::Array(const Array& array) : mElements(array.mElements), mSize(array.mSize) {}


template <typename T>
Array<T>::Array(Array<T>&& rVal) : mElements(std::move(rVal.mElements)) 
{
    mSize = rVal.mSize;
    rVal.mSize = 0;
}

#pragma endregion


#pragma region Array parameters

template <typename T>
inline arr_size_t Array<T>::size() const
{
    return mSize;
}

#pragma endregion


#pragma region Access operators

template <typename T>
inline const T& Array<T>::operator()(arr_size_t index) const
{
    assert_message(index >= 0 && index < mSize, "Array index is out of bounds");
    return mElements[index];
}


template <typename T>
inline T& Array<T>::operator()(arr_size_t index)
{
    assert_message(index >= 0 && index < mSize, "Array index is out of bounds");
    return mElements[index];
}

#pragma endregion


#pragma region Assignment operators

template <typename T>
inline Array<T>& Array<T>::operator=(const Array<T>& r)
{
    mElements = r.mElements;
    mSize = r.mSize;
    return *this;
}


template <typename T>
inline Array<T>& Array<T>::operator=(Array<T>&& rVal)
{
    mElements = std::move(rVal.mElements);
    mSize = rVal.mSize;
    rVal.mSize = 0;
    return *this;
}

#pragma endregion


#pragma region Arithmetic operators

template <typename T>
inline Array<T> operator+(const Array<T>& l, const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator + cannot be applied to Arrays of this type");
    assert_message(l.size() == r.size(), "Operator + cannot be applied to Arrays of different sizes");

    return Array(l.mElements + r.mElements);
}


template <typename T>
inline Array<T> operator-(const Array<T>& l, const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator - cannot be applied to Arrays of this type");
    assert_message(l.size() == r.size(), "Operator - cannot be applied to Arrays of different sizes");

    return Array(l.mElements - r.mElements);
}


template <typename T>
inline Array<T> operator*(const Array<T>& l, const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator * cannot be applied to Arrays of this type");
    assert_message(l.size() == r.size(), "Operator * cannot be applied to Arrays of different sizes");

    return Array(l.mElements * r.mElements);
}


template <typename T>
inline Array<T> operator/(const Array<T>& l, const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator / cannot be applied to Arrays of this type");
    assert_message(l.size() == r.size(), "Operator / cannot be applied to Arrays of different sizes");
    
    return Array(l.mElements / r.mElements);
}


template <typename T, typename K>
inline Array<T> operator*(const Array<T>& l, const K& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator * cannot be applied to Array of this type");
    static_assert(is_arithmetic_ext<K>::value, "Operator * cannot be applied to value of this type");

    Array<T> result(l);

    result *= r;

    return result;
}


template <typename T, typename K>
inline Array<T> operator*(const K& l, const Array<T>& r)
{
    return r * l;
}


template <typename T, typename K>
inline Array<T> operator/(const Array<T>& l, const K& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator / cannot be applied to Array of this type");
    static_assert(is_arithmetic_ext<T>::value, "Operator / cannot be applied to value of this type");

    Array<T> result(l);

    result /= r;

    return result;
}

#pragma endregion


#pragma region Arithmetic operators with assignment

template <typename T>
inline Array<T>& Array<T>::operator+=(const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator += cannot be applied to Arrays of this type");
    assert_message(mSize == r.size(), "Operator += cannot be applied to Arrays of different sizes");

    mElements += r.mElements;

    return *this;
}


template <typename T>
inline Array<T>& Array<T>::operator-=(const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator -= cannot be applied to Arrays of this type");
    assert_message(mSize == r.size(), "Operator -= cannot be applied to Arrays of different sizes");

    mElements -= r.mElements;

    return *this;
}


template <typename T>
inline Array<T>& Array<T>::operator*=(const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator *= cannot be applied to Arrays of this type");
    assert_message(mSize == r.size(), "Operator *= cannot be applied to Arrays of different sizes");

    mElements *= r.mElements;

    return *this;
}


template <typename T>
inline Array<T>& Array<T>::operator/=(const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator /= cannot be applied to Arrays of this type");
    assert_message(mSize == r.size(), "Operator /= cannot be applied to Arrays of different sizes");

    mElements /= r.mElements;

    return *this;
}


template <typename T>
inline Array<T>& Array<T>::operator*=(const T& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator *= cannot be applied to Arrays of this type");

    mElements *= r;

    return *this;
}


template <typename T>
template <typename K>
inline Array<T>& Array<T>::operator*=(const K& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator *= cannot be applied to Array of this type");
    static_assert(is_arithmetic_ext<K>::value, "Operator *= cannot be applied to value of this type");

    for (arr_size_t i = 0; i < mSize; i++)
    {
        mElements[i] *= r;
    }

    return *this;
}


template <typename T>
inline Array<T>& Array<T>::operator/=(const T& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator /= cannot be applied to Array of this type");

    mElements /= r;

    return *this;
}


template <typename T>
template <typename K>
inline Array<T>& Array<T>::operator/=(const K& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator /= cannot be applied to Array of this type");
    static_assert(is_arithmetic_ext<K>::value, "Operator /= cannot be applied to value of this type");

    for (arr_size_t i = 0; i < mSize; i++)
    {
        mElements[i] /= r;
    }

    return *this;
}

#pragma endregion


#pragma region Iterator methods

template <typename T>
inline auto Array<T>::begin()
{
    return std::begin(mElements);
}


template <typename T>
inline const auto Array<T>::begin() const
{
    return std::begin(mElements);
}


template <typename T>
inline auto begin(Array<T>& arr)
{
    return arr.begin();
}


template <typename T>
inline auto Array<T>::end()
{
    return std::end(mElements);
}


template <typename T>
inline const auto Array<T>::end() const
{
    return std::end(mElements);
}


template <typename T>
inline auto end(Array<T>& arr)
{
    return arr.end();
}

#pragma endregion


#pragma region Swap methods

template <typename T>
inline void Array<T>::swap(Array<T>& other)
{
    static_assert(is_swappable_ext<T>::value, "Swap cannot be applied to Arrays of this type");
    std::swap(mElements, other.mElements);
    std::swap(mSize, other.mSize);
}


template <typename T>
inline void swap(Array<T>& first, Array<T>& second)
{
    first.swap(second);
}

#pragma endregion


#pragma region Norms methods

template <typename T>
T norm(const Array<T>& a, const Array<T>& b)
{
    static_assert(std::is_arithmetic<T>::value, "Norm cannot be calculated for arrays of this type");
    assert_message(a.size() == b.size(), "Norm cannot be calculated for arrays of different sizes");

    T max = -std::numeric_limits<T>::min();
    T absDif = 0.0;
    
    const arr_size_t size = a.size();
    for (arr_size_t i = 0; i < size; i++)
    {
        absDif = std::abs(a(i) - b(i));
        if (absDif > max)
        {
            max = absDif;
        }
    }
    
    return max;
}


template <typename T>
T norm(const Array<Vector2<T>>& a, const Array<Vector2<T>>& b)
{
    static_assert(std::is_arithmetic<T>::value, "Norm cannot be calculated for arrays of this type");
    assert_message(a.size() == b.size(), "Norm cannot be calculated for arrays of different sizes");
    
    T max = -std::numeric_limits<T>::min();
    T absDif = 0.0;
    
    const arr_size_t size = a.size();
    for (arr_size_t i = 0; i < size; i++)
    {
        absDif = std::max(std::abs(a(i).x - b(i).x), std::abs(a(i).y - b(i).y));
        
        if (absDif > max)
        {
            max = absDif;
        }
    }
    
    return max;
}

#pragma endregion
