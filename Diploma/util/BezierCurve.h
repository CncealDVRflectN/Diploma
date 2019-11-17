#ifndef DIPLOMA_BEZIER_CURVE_H
#define DIPLOMA_BEZIER_CURVE_H

#include "Array.h"


class BezierCurve
{
public:
    BezierCurve(const Array<Vector2<double>>& points);

    BezierCurve(Array<Vector2<double>>&& points);


    Vector2<double> evaluate(double param) const;

private:
    Array<Vector2<double>> mPoints;
    Array<long long> mCoefficients;


    void calculateCoefficients();
};

#endif