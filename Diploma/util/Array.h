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
    
    #define AUTO_VECTORIZATION
#endif


#if defined(AUTO_VECTORIZATION) && !defined(SIGNED_ARR_SIZE)
    #define SIGNED_ARR_SIZE
#endif


#ifdef SIGNED_ARR_SIZE
    #if defined(_WIN64) && !defined(AUTO_VECTORIZATION)
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

    Array(std::initializer_list<T> initList);

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

    template <typename K>
    Array<T>& operator*=(const K& r);

    Array<T>& operator/=(const T& r);

    template <typename K>
    Array<T>& operator/=(const K& r);


    void swap(Array<T>& other);


    auto begin();

    const auto begin() const;

    auto end();

    const auto end() const;

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

template <typename T, typename K>
Array<T> operator*(const Array<T>& l, const K& r);

template <typename T, typename K>
Array<T> operator*(const K& l, const Array<T>& r);

template <typename T, typename K>
Array<T> operator/(const Array<T>& l, const K& r);


template <typename T>
auto begin(Array<T>& arr);

template <typename T>
auto end(Array<T>& arr);


template <typename T>
T norm(const Array<T>& a, const Array<T>& b);

template <typename T>
T norm(const Array<Vector2<T>>& a, const Array<Vector2<T>>& b);


template <typename T>
void swap(Array<T>& first, Array<T>& second);


#include "Array.tpp"

#endif