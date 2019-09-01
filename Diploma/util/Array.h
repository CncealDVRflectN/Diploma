#ifndef DIPLOMA_ARRAY_H
#define DIPLOMA_ARRAY_H

#include <valarray>

#include "Vector2.h"


#if defined(__AVX2__) || \
    defined(__AVX__) || \
    defined(__SSE4_2__) || \
    defined(__SSE4_1__) || \
    defined(__SSE3__) || \
    defined(__SSE2__) || \
    defined(__SSE__) || \
    defined(_M_IX86_FP) && _M_IX86_FP > 0
    
    #define _AUTO_VECTORIZATION_
#endif


#if defined(_AUTO_VECTORIZATION_) && !defined(_SIGNED_ARR_SIZE_)
    #define _SIGNED_ARR_SIZE_
#endif


#ifdef _SIGNED_ARR_SIZE_
    #if defined(_WIN64) && !defined(_AUTO_VECTORIZATION_)
        typedef __int64 arr_size_t;
    #else
        typedef int     arr_size_t;
    #endif
#else
    typedef size_t arr_size_t;
#endif


template <typename T>
class Array
{
public:
    Array(arr_size_t size);

    Array(const std::valarray<T>& valArray);

    Array(const Array<T>& array);

    Array(Array<T>&& rVal);


    arr_size_t size() const;


    const T& operator()(arr_size_t index) const;

    T& operator()(arr_size_t index);


    Array<T>& operator=(const Array<T>& r);

    Array<T>& operator=(Array<T>&& rVal);


    Array<T>& operator+=(const Array<T>& r);

    Array<T>& operator-=(const Array<T>& r);

    Array<T>& operator*=(const Array<T>& r);

    Array<T>& operator/=(const Array<T>& r);

    Array<T>& operator*=(const T& r);

    Array<T>& operator/=(const T& r);


    void swap(Array<T>& other);

private:
    std::valarray<T> mElements;
    arr_size_t mSize;
};


template <typename T>
Array<T> operator+(const Array<T>& l, const Array<T>& r);

template <typename T>
Array<T> operator-(const Array<T>& l, const Array<T>& r);

template <typename T>
Array<T> operator*(const Array<T>& l, const Array<T>& r);

template <typename T>
Array<T> operator/(const Array<T>& l, const Array<T>& r);

template <typename T>
Array<T> operator*(const Array<T>& l, const T& r);

template <typename T>
Array<T> operator*(const T& l, const Array<T>& r);

template <typename T>
Array<T> operator/(const Array<T>& l, const T& r);


template <typename T>
T norm(const Array<T>& a, const Array<T>& b);

template <typename T>
T norm(const Array<Vector2<T>>& a, const Array<Vector2<T>>& b);


template <typename T>
void swap(Array<T>& first, Array<T>& second);


#include "Array.tpp"

#endif