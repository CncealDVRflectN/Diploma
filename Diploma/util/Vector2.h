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


    Vector2<T>& operator=(const Vector2<T>& r);


	Vector2<T>& operator+=(const Vector2<T>& r);

	Vector2<T>& operator-=(const Vector2<T>& r);

	Vector2<T>& operator*=(const Vector2<T>& r);

	Vector2<T>& operator/=(const Vector2<T>& r);

	Vector2<T>& operator*=(const T& r);

	Vector2<T>& operator/=(const T& r);


    void swap(Vector2<T>& other);
};


template <typename T>
Vector2<T> operator+(const Vector2<T>& l, const Vector2<T>& r);

template <typename T>
Vector2<T> operator-(const Vector2<T>& l, const Vector2<T>& r);

template <typename T>
Vector2<T> operator*(const Vector2<T>& l, const Vector2<T>& r);

template <typename T>
Vector2<T> operator/(const Vector2<T>& l, const Vector2<T>& r);

template <typename T>
Vector2<T> operator*(const Vector2<T>&l, const T& r);

template <typename T>
Vector2<T> operator*(const T& l, const Vector2<T>& r);

template <typename T>
Vector2<T> operator/(const Vector2<T>& l, const T& r);


template <typename T>
void swap(Vector2<T>& first, Vector2<T>& second);


#include "Vector2.tpp"

#endif