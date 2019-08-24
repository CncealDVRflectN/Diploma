#include <algorithm>

#include "Array.h"
#include "type_traits_ext.h"
#include "debug_info.h"



#pragma region Constructors

template <typename T>
Array<T>::Array(size_t size) : elements(size) {}


template <typename T>
Array<T>::Array(const std::valarray<T>& valArray) : elements(valArray) {}


template <typename T>
Array<T>::Array(const Array& array) : elements(array.elements) {}


template <typename T>
Array<T>::Array(Array<T>&& rVal) : elements(std::move(rVal.elements)) {}

#pragma endregion Constructors


#pragma region Array parameters

template <typename T>
inline size_t Array<T>::size() const
{
    return elements.size();
}

#pragma endregion Array parameters


#pragma region Access operators

template <typename T>
inline const T& Array<T>::operator()(size_t index) const
{
    assert_message(index >= 0 && index < elements.size(), "Array index is out of bounds");
    return elements[index];
}


template <typename T>
inline T& Array<T>::operator()(size_t index)
{
    assert_message(index >= 0 && index < elements.size(), "Array index is out of bounds");
    return elements[index];
}

#pragma endregion Access operators


#pragma region Assignment operators

template <typename T>
inline Array<T>& Array<T>::operator=(const Array<T>& r)
{
    elements = r.elements;
}


template <typename T>
inline Array<T>& Array<T>::operator=(Array<T>&& rVal)
{
    elements = std::move(rVal.elements);
}

#pragma endregion Assignment operators


#pragma region Arithmetic operators

template <typename T>
inline Array<T> operator+(const Array<T>& l, const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator + cannot be applied to Arrays of this type");
    assert_message(l.size() == r.size(), "Operator + cannot be applied to Arrays of different sizes");
    return Array(l.elements + r.elements);
}


template <typename T>
inline Array<T> operator-(const Array<T>& l, const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator - cannot be applied to Arrays of this type");
    assert_message(l.size() == r.size(), "Operator - cannot be applied to Arrays of different sizes");
    return Array(l.elements - r.elements);
}


template <typename T>
inline Array<T> operator*(const Array<T>& l, const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator * cannot be applied to Arrays of this type");
    assert_message(l.size() == r.size(), "Operator * cannot be applied to Arrays of different sizes");
    return Array(l.elements * r.elements);
}


template <typename T>
inline Array<T> operator/(const Array<T>& l, const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator / cannot be applied to Arrays of this type");
    assert_message(l.size() == r.size(), "Operator / cannot be applied to Arrays of different sizes");
    return Array(l.elements / r.elements);
}


template <typename T>
inline Array<T> operator*(const Array<T>& l, const T& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator * cannot be applied to Array of this type");

    Array<T> result(elements);

    for (T& val : result)
    {
        val *= r;
    }

    return result;
}


template <typename T>
inline Array<T> operator*(const T& l, const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator * cannot be applied to Array of this type");
    return r * l;
}


template <typename T>
inline Array<T> operator/(const Array<T>& l, const T& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator / cannot be applied to Array of this type");

    Array<T> result(elements);

    for (T& val : result)
    {
        val /= r;
    }

    return result;
}

#pragma endregion Arithmetic operators


#pragma region Arithmetic operators with assignment

template <typename T>
inline Array<T>& Array<T>::operator+=(const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator += cannot be applied to Arrays of this type");
    assert_message(elements.size() == r.size(), "Operator += cannot be applied to Arrays of different sizes");

    elements += r.elements;

    return *this;
}


template <typename T>
inline Array<T>& Array<T>::operator-=(const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator -= cannot be applied to Arrays of this type");
    assert_message(elements.size() == r.size(), "Operator -= cannot be applied to Arrays of different sizes");

    elements -= r.elements;

    return *this;
}


template <typename T>
inline Array<T>& Array<T>::operator*=(const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator *= cannot be applied to Arrays of this type");
    assert_message(elements.size() == r.size(), "Operator *= cannot be applied to Arrays of different sizes");

    elements *= r.elements;

    return *this;
}


template <typename T>
inline Array<T>& Array<T>::operator/=(const Array<T>& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator /= cannot be applied to Arrays of this type");
    assert_message(elements.size() == r.size(), "Operator /= cannot be applied to Arrays of different sizes");

    elements /= r.elements;

    return *this;
}


template <typename T>
inline Array<T>& Array<T>::operator*=(const T& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator *= cannot be applied to Array of this type");

    elements *= r;

    return *this;
}


template <typename T>
inline Array<T>& Array<T>::operator/=(const T& r)
{
    static_assert(is_arithmetic_ext<T>::value, "Operator /= cannot be applied to Array of this type");

    elements /= r;

    return *this;
}

#pragma endregion Arithmetic operators with assignment


#pragma region Swap methods

template <typename T>
inline void Array<T>::swap(Array<T>& other)
{
    static_assert(is_swappable_ext<T>::value, "Swap cannot be applied to Arrays of this type");
    std::swap(elements, other.elements);
}


template <typename T>
inline void swap(Array<T>& first, Array<T>& second)
{
    first.swap(second);
}

#pragma endregion Swap methods


#pragma region Norms methods

template <typename T>
T norm(const Array<T>& a, const Array<T>& b)
{
	static_assert(is_arithmetic_ext<T>::value, "Norm cannot be calculated for arrays of this type");
	assert_message(a.size() == b.size(), "Norm cannot be calculated for arrays of different sizes");

	T max = -std::numeric_limits<T>::min();
	T absDif = 0.0;

	const size_t size = a.size();
	for (size_t i = 0; i < size; i++)
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
	static_assert(is_arithmetic_ext<T>::value, "Norm cannot be calculated for arrays of this type");
	assert_message(a.size() == b.size(), "Norm cannot be calculated for arrays of different sizes");

	T max = -std::numeric_limits<T>::min();
	T absDif = 0.0;

	const size_t size = a.size();
	for (size_t i = 0; i < size; i++)
	{
		absDif = std::max(std::abs(a(i).x - b(i).x), std::abs(a(i).y - b(i).y));

		if (absDif > max)
		{
			max = absDif;
		}
	}

	return max;
}

#pragma endregion Norms methods
