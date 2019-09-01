#ifndef DIPLOMA_TYPE_TRAITS_EXT_H
#define DIPLOMA_TYPE_TRAITS_EXT_H

#include <type_traits>
#include "Vector2.h"
#include "Array.h"
#include "Matrix.h"


#pragma region Arithmetic type trait extension

template <typename T>
struct is_arithmetic_ext
{
    static const bool value = std::is_arithmetic<T>::value;
};


template <typename T>
struct is_arithmetic_ext<Vector2<T>>
{
    static const bool value = std::is_arithmetic<T>::value;
};


template <typename T>
struct is_arithmetic_ext<Array<T>>
{
    static const bool value = std::is_arithmetic<T>::value;
};


template <typename T>
struct is_arithmetic_ext<Matrix<T>>
{
    static const bool value = std::is_arithmetic<T>::value;
};

#pragma endregion


#pragma region Swappable type trait extension

template <typename T>
struct is_swappable_ext
{
    static const bool value = std::is_swappable<T>::value;
};


template <typename T>
struct is_swappable_ext<Vector2<T>>
{
    static const bool value = std::is_swappable<T>::value;
};


template <typename T>
struct is_swappable_ext<Array<T>>
{
    static const bool value = std::is_swappable<T>::value;
};


template <typename T>
struct is_swappable_ext<Matrix<T>>
{
    static const bool value = std::is_swappable<T>::value;
};

#pragma endregion

#endif
