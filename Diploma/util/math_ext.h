#ifndef DIPLOMA_MATH_EXT_H
#define DIPLOMA_MATH_EXT_H

#include "Vector2.h"
#include "Array.h"
#include "Matrix.h"
#include "debug_info.h"
#include "type_traits_ext.h"


#pragma region Lerp

template <typename T>
inline T lerp(const T& start, const T& end, double param)
{
    static_assert(is_arithmetic_ext<T>::value, "Lerp cannot be calculated for values of this type");
    return start + param * (end - start);
}

#pragma endregion Lerp


#pragma region Relaxation

template <typename T>
inline void relaxation(Array<T>& nextApprox, const Array<T>& curApprox, double relaxationCoef)
{
    static_assert(is_arithmetic_ext<T>::value, "Relaxation cannot be calculated for Arrays of this type");
    assert_message(nextApprox.size() == curApprox.size(), "Relaxation cannot be calculated for Arrays of different sizes");

    const size_t size = nextApprox.size();
    for (size_t i = 0; i < size; i++)
    {
        nextApprox(i) = lerp(curApprox(i), nextApprox(i), relaxationCoef);
    }
}


template <typename T>
inline void relaxation(Matrix<T>& nextApprox, const Matrix<T>& curApprox, double relaxationCoef)
{
    static_assert(is_arithmetic_ext<T>::value, "Relaxation cannot be calculated for Matrices of this type");
    assert_message(nextApprox.rowsNum() == curApprox.rowsNum() && nextApprox.columnsNum() == curApprox.columnsNum(), 
                   "Relaxation cannot be calculated for Matrices of different dimentions");

    const size_t elementsNum = nextApprox.elementsNum();
    for (size_t i = 0; i < elementsNum; i++)
    {
        nextApprox(i) = lerp(curApprox(i), nextApprox(i), relaxationCoef);
    }
}

#pragma endregion Relaxation


#pragma region Double triangle area

inline double doubleTriangleArea(const Vector2<double>& vert1, 
                                 const Vector2<double>& vert2, 
                                 const Vector2<double>& vert3)
{
    return (vert1.x - vert2.x) * (vert3.y - vert2.y) - (vert3.x - vert2.x) * (vert1.y - vert2.y);
}

#pragma endregion Double triangle area


#pragma region Parametric point

template <typename T>
T parametric_point(const Array<T>& points, double param)
{
    static_assert(is_arithmetic_ext<T>::value, "Parametric point cannot be calculated for Array of this type");

    const size_t pointsNum = points.size();
    const int maxIndex = pointsNum - 1;

    int prevIndex = (int)(maxIndex * param);
	int nextIndex = prevIndex + 1;

    if (prevIndex >= maxIndex)
    {
        prevIndex = maxIndex;
        nextIndex = prevIndex;
    }

    if (prevIndex < 0)
    {
        prevIndex = 0;
    }

    if (nextIndex < 0)
    {
        nextIndex = 0;
    }

    if (prevIndex == nextIndex)
    {
        return linePoints[prevIndex];
    }

    double prevParam = (double)prevIndex / maxIndex;
    double nextParam = (double)nextIndex / maxIndex;
    double localParam = (param - prevParam) / (nextParam - prevParam);

    return calcLerp(linePoints[prevIndex], linePoints[nextIndex], localParam);
}

#pragma endregion Parametric point

#endif