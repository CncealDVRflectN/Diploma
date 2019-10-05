#include "MagneticFluid.h"
#include "math_ext.h"
#include <algorithm>


#pragma region Constructors

MagneticFluid::MagneticFluid(const FluidParams& params) : mParams(params), 
                                                          mPointsNum(params.splitsNum + 1), 
                                                          mRightSweep(mPointsNum, params.isRightSweepPedantic), 
                                                          mLastValidResult(mPointsNum), 
                                                          mDerivatives(mPointsNum), 
                                                          mNextApproxR(mPointsNum), 
                                                          mNextApproxZ(mPointsNum), 
                                                          mCurApproxR(mPointsNum), 
                                                          mCurApproxZ(mPointsNum), 
                                                          mStep(1.0 / params.splitsNum), 
                                                          mCurRelaxationParam(params.relaxParamInitial), 
                                                          mIterationsCounter(0U)
{}

#pragma endregion


#pragma region Parameters

void MagneticFluid::setRelaxationParam(double param)
{
    mCurRelaxationParam = param;
}


double MagneticFluid::currentRelaxationParam() const
{
    return mCurRelaxationParam;
}


void MagneticFluid::setW(double w)
{
    mParams.w = w;
}


double MagneticFluid::currentW() const
{
    return mParams.w;
}


void MagneticFluid::setChi(double chi)
{
    mParams.chi = chi;
}


double MagneticFluid::currentChi() const
{
    return mParams.chi;
}


FluidParams MagneticFluid::parameters() const
{
    return mParams;
}


arr_size_t MagneticFluid::pointsNum() const
{
    return mPointsNum;
}


void MagneticFluid::setLastValidResult(const Array<Vector2<double>>& values)
{
    mLastValidResult = values;
}


const Array<Vector2<double>>& MagneticFluid::lastValidResult() const
{
    return mLastValidResult;
}


void MagneticFluid::setDerivatives(const Array<Vector2<double>>& values)
{
    mDerivatives = values;
}


double MagneticFluid::volumeNondimMul() const
{
    return 1.0 / cbrt(calcIntegralTrapeze(mLastValidResult));
}


double MagneticFluid::heightCoef() const
{
    return mLastValidResult(0).z / mLastValidResult(mPointsNum - 1).r;
}


void MagneticFluid::resetIterationsCounter()
{
    mIterationsCounter = 0U;
}

#pragma endregion


#pragma region Approximations calculations

void MagneticFluid::calcInitialApproximation()
{
    for (arr_size_t i = 0; i < mPointsNum; i++)
    {
        mLastValidResult(i) = { M_2_PI * sin(M_PI_2 * i * mStep), M_2_PI * cos(M_PI_2 * i * mStep) };
    }
}


ResultCode MagneticFluid::calcRelaxation()
{
    double curEpsilon = mParams.epsilon * mCurRelaxationParam;
    int counter = 0;

    for (arr_size_t i = 0; i < mPointsNum; i++)
    {
        mNextApproxR(i) = mLastValidResult(i).r;
        mNextApproxZ(i) = mLastValidResult(i).z;
    }

    do
    {
        mNextApproxR.swap(mCurApproxR);
        mNextApproxZ.swap(mCurApproxZ);

        calcNextApproximationR(mCurApproxZ, mCurApproxZ);
        relaxation(mNextApproxR, mCurApproxR, mCurRelaxationParam);

        calcNextApproximationZ(mNextApproxR, mCurApproxR);
        relaxation(mNextApproxZ, mCurApproxZ, mCurRelaxationParam);

        counter++;
    } while (std::max(norm(mNextApproxR, mCurApproxR), norm(mNextApproxZ, mNextApproxZ)) > curEpsilon &&
             counter < mParams.iterationsNumMax);

    mIterationsCounter += counter;

    if (counter >= mParams.iterationsNumMax)
    {
        return FLUID_ITERATIONS_LIMIT_EXCEEDED;
    }
    else if (!isApproximationValid(mNextApproxR) || !isApproximationValid(mNextApproxZ))
    {
        return FLUID_INVALID_RESULT;
    }
    else
    {
        for (arr_size_t i = 0; i < mPointsNum; i++)
        {
            mLastValidResult(i).r = mNextApproxR(i);
            mLastValidResult(i).z = mNextApproxZ(i);
        }
    }

    return FLUID_SUCCESS;
}


void MagneticFluid::calcNextApproximationR(const Array<double>& valZ, const Array<double>& prevValZ)
{
    double integralCbrt = cbrt(calcIntegralTrapeze(mCurApproxR, prevValZ));
    double integralCbrtSq = integralCbrt * integralCbrt;
    double valQ = calcQ(mCurApproxR, prevValZ, integralCbrt);
    double tmp = 0.0;

    mRightSweep(RS_MAIN_DIAGONAL, 0) = 1.0;
    mRightSweep(RS_UPPER_DIAGONAL, 0) = 0.0;

    mRightSweep(RS_MAIN_DIAGONAL, 1) = 1.0;
    mRightSweep(RS_UPPER_DIAGONAL, 1) = 0.0;
    mRightSweep(RS_LOWER_DIAGONAL, 0) = 0.0;

    mRightSweep(RS_MAIN_DIAGONAL, mPointsNum - 1) = 1.0;
    mRightSweep(RS_LOWER_DIAGONAL, mPointsNum - 2) = -1.0;

    mRightSweep(RS_CONST_TERMS, 0) = 0.0;
    mRightSweep(RS_CONST_TERMS, 1) = mStep;
    mRightSweep(RS_CONST_TERMS, mPointsNum - 1) = 0.5 * mStep * mStep *
                                                  (valQ - 
                                                  calcMagneticF(mCurApproxR, prevValZ, mPointsNum - 1, integralCbrt) + 
                                                  1.0 / mCurApproxR(mPointsNum - 1));

    arr_size_t limit = mPointsNum - 1;
    for (arr_size_t i = 2; i < limit; i++)
    {
        mRightSweep(RS_LOWER_DIAGONAL, i - 1) = 1.0;
        mRightSweep(RS_MAIN_DIAGONAL, i) = -2.0;
        mRightSweep(RS_UPPER_DIAGONAL, i) = 1.0;

        tmp = 0.5 * (valZ(i + 1) - valZ(i - 1));
        mRightSweep(RS_CONST_TERMS, i) = -mStep * tmp * (valQ - tmp / (mStep * mCurApproxR(i)) - 
                                                         calcMagneticF(mCurApproxR, prevValZ, i, integralCbrt));
    }

    mRightSweep.solve(mNextApproxR);
}


void MagneticFluid::calcNextApproximationZ(const Array<double>& valR, const Array<double>& prevValR)
{
    double integralCbrt = cbrt(calcIntegralTrapeze(prevValR, mCurApproxZ));
    double integralCbrtSq = integralCbrt * integralCbrt;
    double valQ = calcQ(prevValR, mCurApproxZ, integralCbrt);
    double tmp = 0.25 * mStep * mStep;

    mRightSweep(RS_MAIN_DIAGONAL, 0) = -1.0;
    mRightSweep(RS_UPPER_DIAGONAL, 0) = 1.0;

    mRightSweep(RS_MAIN_DIAGONAL, mPointsNum - 2) = 1.0;
    mRightSweep(RS_UPPER_DIAGONAL, mPointsNum - 2) = 0.0;
    mRightSweep(RS_LOWER_DIAGONAL, mPointsNum - 3) = 0.0;

    mRightSweep(RS_MAIN_DIAGONAL, mPointsNum - 1) = 1.0;
    mRightSweep(RS_LOWER_DIAGONAL, mPointsNum - 2) = 0.0;

    mRightSweep(RS_CONST_TERMS, 0) = tmp * (valQ - calcMagneticF(prevValR, mCurApproxZ, 0, integralCbrt));
    mRightSweep(RS_CONST_TERMS, mPointsNum - 2) = mStep * (1.0 - 0.5 * mStep * 
                                                  (valQ -
                                                  calcMagneticF(prevValR, mCurApproxZ, mPointsNum - 1, integralCbrt) + 
                                                  1.0 / prevValR(mPointsNum - 1)));
    mRightSweep(RS_CONST_TERMS, mPointsNum - 1) = 0.0;

    arr_size_t limit = mPointsNum - 2;
    for (arr_size_t i = 1; i < limit; i++)
    {
        mRightSweep(RS_LOWER_DIAGONAL, i - 1) = 1.0 + valR(i - 1) / valR(i);
        mRightSweep(RS_MAIN_DIAGONAL, i) = -(2.0 + (valR(i + 1) + valR(i - 1)) / valR(i));
        mRightSweep(RS_UPPER_DIAGONAL, i) = 1.0 + valR(i + 1) / valR(i);

        mRightSweep(RS_CONST_TERMS, i) = mStep * (valR(i + 1) - valR(i - 1)) * 
                                                 (valQ - calcMagneticF(prevValR, mCurApproxZ, i, integralCbrt));
    }

    mRightSweep.solve(mNextApproxZ);
}

#pragma endregion


#pragma region Integral calculations

double MagneticFluid::calcIntegralTrapeze(const Array<double>& approxR, const Array<double>& approxZ) const
{
    double result = 0.0;
    arr_size_t limit = mPointsNum - 1;

    for (arr_size_t i = 1; i < limit; i++)
    {
        result += approxR(i) * approxZ(i) * (approxR(i + 1) - approxR(i - 1));
    }

    return 2.0 * M_PI * result;
}


double MagneticFluid::calcIntegralTrapeze(const Array<Vector2<double>>& approx) const
{
    double result = 0.0;
    arr_size_t limit = mPointsNum - 1;

    for (arr_size_t i = 1; i < limit; i++)
    {
        result += approx(i).r * approx(i).z * (approx(i + 1).r - approx(i - 1).r);
    }

    return 2.0 * M_PI * result;
}


double MagneticFluid::calcMagneticIntegralTrapeze(const Array<double>& approxR, 
                                                  const Array<double>& approxZ, 
                                                  double integralCbrt) const
{
    double result = 0.0;
    arr_size_t limit = mPointsNum - 1;

    for (arr_size_t i = 1; i < limit; i++)
    {
        result += approxR(i) * (approxR(i + 1) - approxR(i - 1)) * calcMagneticF(approxR, approxZ, i, integralCbrt);
    }

    return 0.5 * result;
}

#pragma endregion 


#pragma region Special functions calculations

double MagneticFluid::calcQ(const Array<double>& approxR, 
                            const Array<double>& approxZ, 
                            double integralCbrt) const
{
    double invRValue = 1.0 / approxR(mPointsNum - 1);
    double magneticIntegral = calcMagneticIntegralTrapeze(approxR, approxZ, integralCbrt);
    return -2.0 * invRValue * (1.0 - magneticIntegral * invRValue);
}


double MagneticFluid::calcMagneticF(const Array<double>& approxR, 
                                    const Array<double>& approxZ, 
                                    int index, 
                                    double integralCbrt) const
{
    double tmp = 0.0;
    Vector2<double> deriv = mDerivatives(index);

    if (index == 0)
    {
        tmp = deriv.z;
    }
    else if (index == mPointsNum - 1)
    {
        tmp = deriv.r;
    }
    else
    {
        tmp = 0.5 * (-(approxZ(index + 1) - approxZ(index - 1)) * deriv.r +
            (approxR(index + 1) - approxR(index - 1)) * deriv.z) / mStep;
    }

    return 0.5 * mParams.w * (tmp * tmp + (deriv.r * deriv.r + deriv.z * deriv.z) / mParams.chi) / integralCbrt;
}

#pragma endregion


#pragma region Validation

bool MagneticFluid::isApproximationValid(const Array<double>& approx) const
{
    for (arr_size_t i = 0; i < mPointsNum; i++)
    {
        if (!std::isfinite(approx(i)) || approx(i) < -0.00001)
        {
            return false;
        }
    }

    return true;
}

#pragma endregion
