#ifndef DIPLOMA_ARRAY_H
#define DIPLOMA_ARRAY_H

#include <valarray>

#include "Vector2.h"



template <typename T>
class Array
{
public:
    Array(size_t size);

    Array(const std::valarray<T>& valArray);

    Array(const Array<T>& array);

    Array(Array<T>&& rVal);


    size_t size() const;


    const T& operator()(size_t index) const;

    T& operator()(size_t index);


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
    std::valarray<T> elements;
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