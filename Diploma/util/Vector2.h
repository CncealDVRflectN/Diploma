#ifndef DIPLOMA_VECTOR2_H
#define DIPLOMA_VECTOR2_H


template <typename T>
class Vector2
{
public:
    union
    {
        struct
        {
            T x;
            T y;
        };
        
        struct
        {
            T r;
            T z;
        };
        
        struct
        {
            T i;
            T j;
        };
        
        struct
        {
            T u;
            T v;
        };
        
        T raw[2];
    };
    
    
    Vector2();
    
    Vector2(const T& xVal, const T& yVal);
    
    Vector2(const Vector2<T>& vect);
    
    Vector2(std::initializer_list<T> initList);
    
    
    double length() const;

    template <typename K>
    Vector2<T>& operator=(const Vector2<K>& r);


    template <typename K>
    Vector2<T>& operator+=(const Vector2<K>& r);

    template <typename K>
    Vector2<T>& operator-=(const Vector2<K>& r);

    template <typename K>
    Vector2<T>& operator*=(const Vector2<K>& r);

    template <typename K>
    Vector2<T>& operator/=(const Vector2<K>& r);

    template <typename K>
    Vector2<T>& operator*=(const K& r);

    template <typename K>
    Vector2<T>& operator/=(const K& r);
    
    
    void swap(Vector2<T>& other);
};


template <typename T, typename K>
Vector2<T> operator+(const Vector2<T>& l, const Vector2<K>& r);

template <typename T, typename K>
Vector2<T> operator-(const Vector2<T>& l, const Vector2<K>& r);

template <typename T, typename K>
Vector2<T> operator*(const Vector2<T>& l, const Vector2<K>& r);

template <typename T, typename K>
Vector2<T> operator/(const Vector2<T>& l, const Vector2<K>& r);

template <typename T, typename K>
Vector2<T> operator*(const Vector2<T>&l, const K& r);

template <typename T, typename K>
Vector2<T> operator*(const K& l, const Vector2<T>& r);

template <typename T, typename K>
Vector2<T> operator/(const Vector2<T>& l, const K& r);


template <typename T>
void swap(Vector2<T>& first, Vector2<T>& second);


#include "Vector2.tpp"

#endif