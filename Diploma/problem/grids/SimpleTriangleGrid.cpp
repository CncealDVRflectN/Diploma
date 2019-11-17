#include "SimpleTriangleGrid.h"
#include "math_ext.h"


#pragma region Constructors

SimpleTriangleGrid::SimpleTriangleGrid(const STGridParams& params) 
    : mParams(params), 
      mSurfaceColumnIndex(params.internalSplitsNum), 
      mPoints(params.surfaceSplitsNum + 1, params.internalSplitsNum + params.externalSplitsNum + 1)
{}

#pragma endregion


#pragma region Grid parameters

arr_size_t SimpleTriangleGrid::rowsNum() const
{
    return mPoints.rowsNum();
}


arr_size_t SimpleTriangleGrid::columnsNum() const
{
    return mPoints.columnsNum();
}


arr_size_t SimpleTriangleGrid::pointsNum() const
{
    return mPoints.elementsNum();
}


arr_size_t SimpleTriangleGrid::surfaceColumnsIndex() const
{
    return mSurfaceColumnIndex;
}


STGridParams SimpleTriangleGrid::parameters() const
{
    return mParams;
}

#pragma endregion


#pragma region Raw data

const Matrix<Vector2<double>>& SimpleTriangleGrid::rawPoints() const
{
    return mPoints;
}

#pragma endregion


#pragma region Access operators

Vector2<double> SimpleTriangleGrid::operator()(arr_size_t row, arr_size_t column) const
{
    return mPoints(row, column);
}


Vector2<double> SimpleTriangleGrid::operator()(const Vector2<arr_size_t>& indices) const
{
    return mPoints(indices);
}

#pragma endregion


#pragma region Generation methods

void SimpleTriangleGrid::generate(const Array<double>& surfacePointsR, const Array<double>& surfacePointsZ)
{
    assert_message(surfacePointsR.size() == surfacePointsZ.size(), 
                   "SimpleTriangleGrid cannot be generated because surfacePointsR and surfacePointsZ have different sizes");

    arr_size_t pointsNum = surfacePointsR.size();
    Array<Vector2<double>> surfacePoints(pointsNum);

    for (arr_size_t i = 0; i < pointsNum; i++)
    {
        surfacePoints(i).r = surfacePointsR(i);
        surfacePoints(i).z = surfacePointsZ(i);
    }

    generate(surfacePoints);
}


void SimpleTriangleGrid::generate(const Array<Vector2<double>>& surfacePoints)
{
    Vector2<double> surfacePoint;
    double topPointZ = surfacePoints(0).z;
    double symmetryStep = topPointZ / (mParams.surfaceSplitsNum + mParams.internalSplitsNum);
    double specialPointZ = symmetryStep * mParams.surfaceSplitsNum;
    double param = 0.0;

    arr_size_t rowsNum = mPoints.rowsNum();
    arr_size_t columnsNum = mPoints.columnsNum();
    arr_size_t maxRowIndex = rowsNum - 1;
    arr_size_t maxColumnIndex = columnsNum - 1;

    for (arr_size_t i = 0; i < rowsNum; i++)
    {
        param = (double)i / maxRowIndex;
        surfacePoint = parametric_point(surfacePoints, 1.0 - param);

        mPoints(i, 0) = { 0.0, specialPointZ * param };
        mPoints(i, mSurfaceColumnIndex) = surfacePoint;
        mPoints(i, maxColumnIndex) = mParams.infMultiplier * surfacePoint;
    }

    double intersectPointR = surfacePoints(surfacePoints.size() - 1).r;

    for (arr_size_t i = 1; i < mSurfaceColumnIndex; i++)
    {
        param = (double)i / mSurfaceColumnIndex;

        mPoints(0, i) = { intersectPointR * param, 0.0 };
        mPoints(maxRowIndex, i) = { 0.0, lerp(specialPointZ, topPointZ, param) };
    }

    double infTopPointZ = mParams.infMultiplier * topPointZ;
    double infIntersectPointR = mParams.infMultiplier * intersectPointR;
    arr_size_t infColumnsNum = maxColumnIndex - mSurfaceColumnIndex;

    for (arr_size_t i = mSurfaceColumnIndex + 1; i < columnsNum; i++)
    {
        param = (double)(i - mSurfaceColumnIndex) / infColumnsNum;

        mPoints(0, i) = { lerp(intersectPointR, infIntersectPointR, param), 0.0 };
        mPoints(maxRowIndex, i) = { 0.0, lerp(topPointZ, infTopPointZ, param) };
    }

    for (arr_size_t i = 1; i < maxRowIndex; i++)
    {
        for (arr_size_t j = 1; j < mSurfaceColumnIndex; j++)
        {
            param = (double)j / mSurfaceColumnIndex;

            mPoints(i, j) = lerp(mPoints(i, 0), mPoints(i, mSurfaceColumnIndex), param);
        }
        
        for (arr_size_t j = mSurfaceColumnIndex + 1; j < maxColumnIndex; j++)
        {
            param = (double)(j - mSurfaceColumnIndex) / infColumnsNum;

            mPoints(i, j) = lerp(mPoints(i, mSurfaceColumnIndex), mPoints(i, maxColumnIndex), param);
        }
    }
}

#pragma endregion
