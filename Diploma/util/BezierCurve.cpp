#include "BezierCurve.h"


#pragma region Constructors

BezierCurve::BezierCurve(const Array<Vector2<double>>& points) : mPoints(points), mCoefficients(points.size())
{
    calculateCoefficients();
}


BezierCurve::BezierCurve(Array<Vector2<double>>&& points) : mPoints(std::move(points)), mCoefficients(mPoints.size())
{
    calculateCoefficients();
}

#pragma endregion



#pragma region Calculations

Vector2<double> BezierCurve::evaluate(double param) const
{
    arr_size_t pointsNum = mPoints.size();
    double curParam = 1.0;
    double curOneMinusParam = std::pow(1.0 - param, pointsNum - 1);
    double oneMinusParamInv = 1.0 / (1.0 - param);
    Vector2<double> result(0.0, 0.0);

    for (arr_size_t i = 0; i < pointsNum; i++)
    {
        result += mPoints(i) * mCoefficients(i) * curParam * curOneMinusParam;

        curParam *= param;
        curOneMinusParam *= oneMinusParamInv;
    }

    return result;
}


void BezierCurve::calculateCoefficients()
{
    arr_size_t coeffsNum = mCoefficients.size();
    arr_size_t curveOrder = coeffsNum - 1;
    arr_size_t halfCeil = (coeffsNum + 1) / 2;
    arr_size_t halfFloor = coeffsNum / 2;
    
    mCoefficients(0) = 1;
    for (arr_size_t i = 1; i < halfCeil; i++)
    {
        mCoefficients(i) = mCoefficients(i - 1) * (coeffsNum - i) / i;
    }

    for (arr_size_t i = 0; i < halfFloor; i++)
    {
        mCoefficients(curveOrder - i) = mCoefficients(i);
    }
}

#pragma endregion
