#include "MagneticField.h"
#include "math_ext.h"


static const Array<Vector2<arr_size_t>> OFFSETS_TABLE = { {-1,  1}, 
										                  { 0,  1}, 
										                  { 1,  0}, 
										                  { 1, -1}, 
										                  { 0, -1}, 
										                  {-1,  0}, 
										                  {-1,  1}, 
										                  { 0,  1} };


#pragma region Constructors

MagneticField::MagneticField(const MagneticParams& params) : mParams(params), 
                                                             mGrid(params.gridParams), 
                                                             mLastValidValues(mGrid.rowsNum(), mGrid.columnsNum()), 
                                                             mCurApprox(mGrid.rowsNum(), mGrid.columnsNum()), 
                                                             mNextApprox(mGrid.rowsNum(), mGrid.columnsNum()), 
                                                             mInnerDerivatives(mGrid.rowsNum()), 
                                                             mOuterDerivatives(mGrid.rowsNum()), 
                                                             mCurRelaxationParam(params.relaxParamInitial), 
                                                             mIterationsCounter(0U)
{}

#pragma endregion


#pragma region Parameters

void MagneticField::setChi(double chi)
{
    mParams.chi = chi;
}


double MagneticField::currentChi() const
{
    return mParams.chi;
}


void MagneticField::setRelaxationParam(double param)
{
    mCurRelaxationParam = param;
}


double MagneticField::currentRelaxationParam() const
{
    return mCurRelaxationParam;
}


MagneticParams MagneticField::parameters() const
{
    return mParams;
}


void MagneticField::setGrid(const SimpleTriangleGrid& grid)
{
    mGrid = grid;
}


const SimpleTriangleGrid& MagneticField::grid() const
{
    return mGrid;
}


void MagneticField::setLastValidResult(const Matrix<double>& values)
{
    mLastValidValues = values;
}


const Matrix<double>& MagneticField::lastValidResult() const
{
    return mLastValidValues;
}


const Array<Vector2<double>>& MagneticField::innerDerivatives() const
{
    return mInnerDerivatives;
}


const Array<Vector2<double>>& MagneticField::outerDerivatives() const
{
    return mOuterDerivatives;
}


void MagneticField::resetIterationsCounter()
{
    mIterationsCounter = 0U;
}

#pragma endregion


#pragma region Grid update

void MagneticField::updateGrid(const Array<Vector2<double>>& surfacePoints)
{
    mGrid.generate(surfacePoints);
}

#pragma endregion


#pragma region Coefficients calculations

double MagneticField::calcCoefficientIntegral(const Vector2<double>& vert1, 
                                              const Vector2<double>& vert2, 
                                              const Vector2<double>& vert3, 
                                              double doubleTriangleArea, 
                                              double chi) const
{
    return doubleTriangleArea * (1.0 + chi) * (vert1.r + vert2.r + vert3.r) / 6.0;
}


double MagneticField::calcCoefficient(const Vector2<arr_size_t>& globIndex, arr_size_t coefIndex)
{
    return calcCoefficient(globIndex.i, globIndex.j, coefIndex);
}


double MagneticField::calcCoefficient(arr_size_t i, arr_size_t j, arr_size_t coefIndex)
{
    double result = 0.0;
    double doubleArea = 0.0;
    double integralVal = 0.0;
    double chi = 0.0;

    arr_size_t surfaceColumnIndex = mGrid.surfaceColumnsIndex();

    Vector2<arr_size_t> curIndices;
    Vector2<arr_size_t> nextIndices;
    Vector2<double> vert1 = mGrid(i, j);
    Vector2<double> vert2;
    Vector2<double> vert3;

    if (coefIndex == 0)
    {
        double difR = 0.0;
        double difZ = 0.0;

        for (arr_size_t k = 1; k < 7; k++)
        {
            curIndices = OFFSETS_TABLE(k);
            nextIndices = OFFSETS_TABLE(k + 1);

            curIndices.i += i;
            curIndices.j += j;

            nextIndices.i += i;
            nextIndices.j += j;

            if (!isIndicesValid(curIndices) || !isIndicesValid(nextIndices))
            {
                continue;
            }

            vert2 = mGrid(curIndices);
            vert3 = mGrid(nextIndices);

            chi = (curIndices.j > surfaceColumnIndex || 
                   nextIndices.j > surfaceColumnIndex || 
                   j > surfaceColumnIndex) ? 0.0 : mParams.chi;

            doubleArea = double_triangle_area(vert1, vert2, vert3);
            integralVal = calcCoefficientIntegral(vert1, vert2, vert3, doubleArea, chi);

            difZ = vert3.z - vert2.z;
            difR = vert3.r - vert2.r;

            result += (difZ * difZ + difR * difR) * integralVal / (doubleArea * doubleArea);
        }
    }
    else
    {
        curIndices = OFFSETS_TABLE(coefIndex - 1);
        nextIndices = OFFSETS_TABLE(coefIndex);

        curIndices.i += i;
        curIndices.j += j;

        nextIndices.i += i;
        nextIndices.j += j;

        if (isIndicesValid(curIndices) && isIndicesValid(nextIndices))
        {
            vert2 = mGrid(curIndices);
            vert3 = mGrid(nextIndices);

            chi = (curIndices.j > surfaceColumnIndex || 
                   nextIndices.j > surfaceColumnIndex || 
                   j > surfaceColumnIndex) ? 0.0 : mParams.chi;

            doubleArea = double_triangle_area(vert1, vert2, vert3);
            integralVal = calcCoefficientIntegral(vert1, vert2, vert3, doubleArea, chi);

            result += ((vert3.z - vert2.z) * (vert2.z - vert1.z) + (vert3.r - vert2.r) * (vert2.r - vert1.r)) *
                integralVal / (doubleArea * doubleArea);
        }

        curIndices = OFFSETS_TABLE(coefIndex);
        nextIndices = OFFSETS_TABLE(coefIndex + 1);

        curIndices.i += i;
        curIndices.j += j;

        nextIndices.i += i;
        nextIndices.j += j;

        if (isIndicesValid(curIndices) && isIndicesValid(nextIndices))
        {
            vert2 = mGrid(curIndices);
            vert3 = mGrid(nextIndices);

            chi = (curIndices.j > surfaceColumnIndex ||
                   nextIndices.j > surfaceColumnIndex || 
                   j > surfaceColumnIndex) ? 0.0 : mParams.chi;

            doubleArea = double_triangle_area(vert1, vert2, vert3);
            integralVal = calcCoefficientIntegral(vert1, vert2, vert3, doubleArea, chi);

            result += ((vert3.z - vert2.z) * (vert1.z - vert3.z) + (vert3.r - vert2.r) * (vert1.r - vert3.r)) *
                integralVal / (doubleArea * doubleArea);
        }
    }

    return result;
}

#pragma endregion


#pragma region Approximations calculations

void MagneticField::calcInitialApproximation()
{
    arr_size_t gridRowsNum = mGrid.rowsNum();
    arr_size_t gridColumnsNum = mGrid.columnsNum();

    for (arr_size_t i = 0; i < gridRowsNum; i++)
    {
        for (arr_size_t j = 0; j < gridColumnsNum; j++)
        {
            mLastValidValues(i, j) = 0.0;
        }
    }
}


double MagneticField::calcNextValue(const Vector2<arr_size_t>& globIndex)
{
    return calcNextValue(globIndex.i, globIndex.j);
}


double MagneticField::calcNextValue(arr_size_t i, arr_size_t j)
{
    double result = 0.0;
    Vector2<arr_size_t> resultIndices;

    for (arr_size_t k = 2; k < 5; k++)
    {
        resultIndices = OFFSETS_TABLE(k);

        resultIndices.i += i;
        resultIndices.j += j;

        if (isIndicesValid(resultIndices))
        {
            result += calcCoefficient(i, j, k) * mCurApprox(resultIndices.i, resultIndices.j);
        }
    }

    for (arr_size_t k = 5; k < 7; k++)
    {
        resultIndices = OFFSETS_TABLE(k);

        resultIndices.i += i;
        resultIndices.j += j;

        if (isIndicesValid(resultIndices))
        {
            result += calcCoefficient(i, j, k) * mNextApprox(resultIndices.i, resultIndices.j);
        }
    }

    resultIndices = OFFSETS_TABLE(1);

    resultIndices.i += i;
    resultIndices.j += j;

    if (isIndicesValid(resultIndices))
    {
        result += calcCoefficient(i, j, 1) * mNextApprox(resultIndices.i, resultIndices.j);
    }

    return -result / calcCoefficient(i, j, 0);
}


void MagneticField::calcNextApproximation()
{
    arr_size_t gridRowsNum = mGrid.rowsNum();
    arr_size_t gridColumnsNum = mGrid.columnsNum();

    for (arr_size_t i = 1; i < gridRowsNum; i++)
    {
        for (arr_size_t j = gridColumnsNum - 2; j >= 0; j--)
        {
            mNextApprox(i, j) = calcNextValue(i, j);
        }
    }
}


ResultCode MagneticField::calcRelaxation()
{
    unsigned int counter = 0U;
    double curEpsilon = mParams.accuracy * mCurRelaxationParam;
    arr_size_t gridRowsNum = mGrid.rowsNum();
    arr_size_t gridColumnsNum = mGrid.columnsNum();
    arr_size_t limitColumns = gridColumnsNum - 1;

    for (arr_size_t i = 0; i < gridRowsNum; i++)
    {
        for (arr_size_t j = 0; j < limitColumns; j++)
        {
            mNextApprox(i, j) = mLastValidValues(i, j);
            mCurApprox(i, j) = mNextApprox(i, j);
        }
    }

    for (arr_size_t i = 0; i < gridRowsNum; i++)
    {
        mNextApprox(i, limitColumns) = mGrid(i, limitColumns).z;
        mCurApprox(i, limitColumns) = mGrid(i, limitColumns).z;
    }

    do
    {
        swap(mNextApprox, mCurApprox);

        calcNextApproximation();
        relaxation(mNextApprox, mCurApprox, mCurRelaxationParam);

        counter++;
    } while (norm(mNextApprox, mCurApprox) > curEpsilon);

    mIterationsCounter += counter;

    if (counter >= mParams.iterationsNumMax)
    {
        return FIELD_ITERATIONS_LIMIT_EXCEEDED;
    }
    else if (!isApproximationValid(mNextApprox))
    {
        return FIELD_INVALID_RESULT;
    }
    else
    {
        mLastValidValues.swap(mNextApprox);

        calcDerivatives();
    }

    return FIELD_SUCCESS;
}

#pragma endregion


#pragma region Validation

bool MagneticField::isApproximationValid(const Matrix<double>& approx) const
{
    arr_size_t rowsNum = approx.rowsNum();
    arr_size_t columnsNum = approx.columnsNum();

    for (arr_size_t i = 0; i < rowsNum; i++)
    {
        for (arr_size_t j = 0; j < columnsNum; j++)
        {
            if (!std::isfinite(approx(i, j)) || approx(i, j) < -0.00001)
            {
                return false;
            }
        }
    }

    return true;
}


bool MagneticField::isIndicesValid(const Vector2<arr_size_t>& indices) const
{
    return indices.i >= 0 && indices.j >= 0 && 
           indices.i < mGrid.rowsNum() && indices.j < mGrid.columnsNum();
}

#pragma endregion


#pragma region Derivatives calculations

Vector2<double> MagneticField::calcInnerDerivative(double param) const
{
    return parametric_point(mInnerDerivatives, (1.0 - param));
}


Vector2<double> MagneticField::calcOuterDerivative(double param) const
{
    return parametric_point(mOuterDerivatives, (1.0 - param));
}


void MagneticField::calcDerivatives()
{
    arr_size_t surfacePointsNum = mParams.gridParams.surfaceSplitsNum + 1;
    arr_size_t limit = mParams.gridParams.surfaceSplitsNum;
    arr_size_t surfaceColumnIndex = mGrid.surfaceColumnsIndex();
    double doubleArea = 0.0;

    Vector2<arr_size_t> indicesOffset;

    Vector2<double> vert1 = mGrid(0, surfaceColumnIndex);
    Vector2<double> vertMagneticR1 = { mLastValidValues(0, surfaceColumnIndex), vert1.z };
    Vector2<double> vertMagneticZ1 = { vert1.r, mLastValidValues(0, surfaceColumnIndex) };

    indicesOffset = OFFSETS_TABLE(1);

    Vector2<double> vert2 = mGrid(indicesOffset.i, surfaceColumnIndex + indicesOffset.j);
    Vector2<double> vertMagneticR2 = { mLastValidValues(indicesOffset.i, surfaceColumnIndex + indicesOffset.j), vert2.z };
    Vector2<double> vertMagneticZ2 = { vert2.r, mLastValidValues(0 + indicesOffset.i, surfaceColumnIndex + indicesOffset.j) };

    indicesOffset = OFFSETS_TABLE(2);

    Vector2<double> vert3 = mGrid(indicesOffset.i, surfaceColumnIndex + indicesOffset.j);
    Vector2<double> vertMagneticR3 = { mLastValidValues(indicesOffset.i, surfaceColumnIndex + indicesOffset.j), vert3.z };
    Vector2<double> vertMagneticZ3 = { vert3.r, mLastValidValues(0 + indicesOffset.i, surfaceColumnIndex + indicesOffset.j) };

    doubleArea = double_triangle_area(vert1, vert2, vert3);

    mOuterDerivatives(0).r = 0.0;
    mOuterDerivatives(0).z = double_triangle_area(vertMagneticZ1, vertMagneticZ2, vertMagneticZ3) / doubleArea;

    for (arr_size_t i = 0; i < limit; i++)
    {
        vert1 = mGrid(i, surfaceColumnIndex);
        vertMagneticR1 = { mLastValidValues(i, surfaceColumnIndex), vert1.z };
        vertMagneticZ1 = { vert1.r, mLastValidValues(i, surfaceColumnIndex) };

        indicesOffset = OFFSETS_TABLE(3);

        vert2 = mGrid(i + indicesOffset.i, surfaceColumnIndex + indicesOffset.j);
        vertMagneticR2 = { mLastValidValues(i + indicesOffset.i, surfaceColumnIndex + indicesOffset.j), vert2.z };
        vertMagneticZ2 = { vert2.r, mLastValidValues(i + indicesOffset.i, surfaceColumnIndex + indicesOffset.j) };

        indicesOffset = OFFSETS_TABLE(4);

        vert3 = mGrid(i + indicesOffset.i, surfaceColumnIndex + indicesOffset.j);
        vertMagneticR3 = { mLastValidValues(i + indicesOffset.i, surfaceColumnIndex + indicesOffset.j), vert3.z };
        vertMagneticZ3 = { vert3.r, mLastValidValues(i + indicesOffset.i, surfaceColumnIndex + indicesOffset.j) };

        doubleArea = double_triangle_area(vert1, vert2, vert3);

        mInnerDerivatives(i).r = double_triangle_area(vertMagneticR1, vertMagneticR2, vertMagneticR3) / doubleArea;
        mInnerDerivatives(i).z = double_triangle_area(vertMagneticZ1, vertMagneticZ2, vertMagneticZ3) / doubleArea;
    }


    for (arr_size_t i = 1; i < surfacePointsNum; i++)
    {
        vert1 = mGrid(i, surfaceColumnIndex);
        vertMagneticR1 = { mLastValidValues(i, surfaceColumnIndex), vert1.z };
        vertMagneticZ1 = { vert1.r, mLastValidValues(i, surfaceColumnIndex) };

        indicesOffset = OFFSETS_TABLE(6);

        vert2 = mGrid(i + indicesOffset.i, surfaceColumnIndex + indicesOffset.j);
        vertMagneticR2 = { mLastValidValues(i + indicesOffset.i, surfaceColumnIndex + indicesOffset.j), vert2.z };
        vertMagneticZ2 = { vert2.r, mLastValidValues(i + indicesOffset.i, surfaceColumnIndex + indicesOffset.j) };

        indicesOffset = OFFSETS_TABLE(1);

        vert3 = mGrid(i + indicesOffset.i, surfaceColumnIndex + indicesOffset.j);
        vertMagneticR3 = { mLastValidValues(i + indicesOffset.i, surfaceColumnIndex + indicesOffset.j), vert3.z };
        vertMagneticZ3 = { vert3.r, mLastValidValues(i + indicesOffset.i, surfaceColumnIndex + indicesOffset.j) };

        doubleArea = double_triangle_area(vert1, vert2, vert3);

        mOuterDerivatives(i).r = double_triangle_area(vertMagneticR1, vertMagneticR2, vertMagneticR3) / doubleArea;
        mOuterDerivatives(i).z = double_triangle_area(vertMagneticZ1, vertMagneticZ2, vertMagneticZ3) / doubleArea;
    }

    vert1 = mGrid(limit, surfaceColumnIndex);
    vertMagneticR1 = { mLastValidValues(limit, surfaceColumnIndex), vert1.z };
    vertMagneticZ1 = { vert1.r, mLastValidValues(limit, surfaceColumnIndex) };

    indicesOffset = OFFSETS_TABLE(4);

    vert2 = mGrid(limit + indicesOffset.i, surfaceColumnIndex + indicesOffset.j);
    vertMagneticR2 = { mLastValidValues(limit + indicesOffset.i, surfaceColumnIndex + indicesOffset.j), vert2.z };
    vertMagneticZ2 = { vert2.r, mLastValidValues(limit + indicesOffset.i, surfaceColumnIndex + indicesOffset.j) };

    indicesOffset = OFFSETS_TABLE(5);

    vert3 = mGrid(limit + indicesOffset.i, surfaceColumnIndex + indicesOffset.j);
    vertMagneticR3 = { mLastValidValues(limit + indicesOffset.i, surfaceColumnIndex + indicesOffset.j), vert3.z };
    vertMagneticZ3 = { vert3.r, mLastValidValues(limit + indicesOffset.i, surfaceColumnIndex + indicesOffset.j) };

    doubleArea = double_triangle_area(vert1, vert2, vert3);

    mInnerDerivatives(limit).r = 0.0;
    mInnerDerivatives(limit).z = double_triangle_area(vertMagneticZ1, vertMagneticZ2, vertMagneticZ3) / doubleArea;
}

#pragma endregion
