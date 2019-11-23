#include "Vector2.h"
#include "type_traits_ext.h"
#include "debug_info.h"


#pragma region Type traits extension

template <typename T>
struct is_arithmetic_ext<Vector2<T>>
{
    static const bool value = std::is_arithmetic<T>::value;
};


template <typename T>
struct is_swappable_ext<Vector2<T>>
{
    static const bool value = std::is_swappable<T>::value;
};

#pragma endregion


#pragma region Constructors

template <typename T>
Vector2<T>::Vector2() : x(0), y(0) 
{
    static_assert(std::is_arithmetic<T>::value, "Vector2 cannot be constructed for non-arithmetic types");
}


template <typename T>
Vector2<T>::Vector2(const T& xVal, const T& yVal) : x(xVal), y(yVal) 
{
    static_assert(std::is_arithmetic<T>::value, "Vector2 cannot be constructed for non-arithmetic types");
}


template <typename T>
Vector2<T>::Vector2(const Vector2<T>& vect) : x(vect.x), y(vect.y) {}


template <typename T>
Vector2<T>::Vector2(std::initializer_list<T> initList)
{
    static_assert(std::is_arithmetic<T>::value, "Vector2 cannot be constructed for non-arithmetic types");
    assert_message(initList.size() == 2, "Initializer list size must be 2");

    x = *(initList.begin());
    y = *(initList.begin() + 1);
}

#pragma endregion


#pragma region Mathematical methods

template <typename T>
inline double Vector2<T>::length() const
{
    static_assert(std::is_arithmetic<T>::value, "Length cannot be calculated for Vector2 of this type");
    return sqrt(x * x + y * y);
}

#pragma endregion


#pragma region Assignment operators

template <typename T>
template <typename K>
inline Vector2<T>& Vector2<T>::operator=(const Vector2<K>& r)
{
    x = r.x;
    y = r.y;

    return *this;
}

#pragma endregion


#pragma region Arithmetic operators

template <typename T, typename K>
inline Vector2<T> operator+(const Vector2<T>& l, const Vector2<K>& r)
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<K>::value, 
                  "Operator + cannot be calculated for Vector2 of this type");
    return Vector2<T>(l.x + r.x, l.y + r.y);
}


template <typename T, typename K>
inline Vector2<T> operator-(const Vector2<T>& l, const Vector2<K>& r)
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<K>::value, 
                  "Operator - cannot be calculated for Vector2 of this type");
    return Vector2<T>(l.x - r.x, l.y - r.y);
}


template <typename T, typename K>
inline Vector2<T> operator*(const Vector2<T>& l, const Vector2<K>& r)
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<K>::value, 
                  "Operator * cannot be calculated for Vector2 of this type");
    return Vector2<T>(l.x * r.x, l.y * r.y);
}


template <typename T, typename K>
inline Vector2<T> operator/(const Vector2<T>& l, const Vector2<K>& r)
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<K>::value, 
                  "Operator / cannot be calculated for Vector2 of this type");
    return Vector2<T>(l.x / r.x, l.y / r.y);
}


template <typename T, typename K>
inline Vector2<T> operator*(const Vector2<T>& l, const K& r)
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<K>::value, 
                  "Operator * cannot be calculated for Vector2 of this type");
    return Vector2<T>(l.x * r, l.y * r);
}


template <typename T, typename K>
inline Vector2<T> operator*(const K& l, const Vector2<T>& r)
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<K>::value, 
                  "Operator * cannot be calculated for Vector2 of this type");
    return r * l;
}


template <typename T, typename K>
inline Vector2<T> operator/(const Vector2<T>& l, const K& r)
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<K>::value, 
                  "Operator / cannot be calculated for Vector2 of this type");
    return Vector2<T>(l.x / r, l.y / r);
}

#pragma endregion


#pragma region Arithmetic operators with assignment

template <typename T>
template <typename K>
inline Vector2<T>& Vector2<T>::operator+=(const Vector2<K>& r)
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<K>::value, 
                  "Operator += cannot be calculated for Vector2 of this type");
    
    x += r.x;
    y += r.y;
    
    return *this;
}


template <typename T>
template <typename K>
inline Vector2<T>& Vector2<T>::operator-=(const Vector2<K>& r)
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<K>::value, 
                  "Operator -= cannot be calculated for Vector2 of this type");
    
    x -= r.x;
    y -= r.y;
    
    return *this;
}


template <typename T>
template <typename K>
inline Vector2<T>& Vector2<T>::operator*=(const Vector2<K>& r)
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<K>::value, 
                  "Operator *= cannot be calculated for Vector2 of this type");
    
    x *= r.x;
    y *= r.y;
    
    return *this;
}


template <typename T>
template <typename K>
inline Vector2<T>& Vector2<T>::operator/=(const Vector2<K>& r)
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<K>::value, 
                  "Operator /= cannot be calculated for Vector2 of this type");
    
    x /= r.x;
    y /= r.y;
    return *this;
}


template <typename T>
template <typename K>
inline Vector2<T>& Vector2<T>::operator*=(const K& r)
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<K>::value, 
                  "Operator *= cannot be calculated for Vector2 of this type");
    
    x *= r;
    y *= r;
    
    return *this;
}


template <typename T>
template <typename K>
inline Vector2<T>& Vector2<T>::operator/=(const K& r)
{
    static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<K>::value, 
                  "Operator /= cannot be calculated for Vector2 of this type");
    
    x /= r;
    y /= r;
    
    return *this;
}

#pragma endregion


#pragma region Swap methods

template <typename T>
inline void Vector2<T>::swap(Vector2<T>& other)
{
    static_assert(is_swappable_ext<T>::value, "Swap cannot be applied to Vector2 of this type");
    std::swap(x, other.x);
    std::swap(y, other.y);
}


template <typename T>
inline void swap(Vector2<T>& first, Vector2<T>& second)
{
    first.swap(second);
}

#pragma endregion
