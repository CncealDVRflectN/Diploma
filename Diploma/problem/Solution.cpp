#include "Solution.h"


#pragma region Parameters parsing

FluidParams getFluidParams(const ProblemParams& problemParams)
{
    FluidParams fluidParams;

    fluidParams.w = 0.0;
    fluidParams.chi = problemParams.chi;
    fluidParams.epsilon = problemParams.accuracy;
    fluidParams.iterationsNumMax = problemParams.iterationsMaxNum;
    fluidParams.relaxParamInitial = problemParams.relaxationParamInitial;
    fluidParams.relaxParamMin = problemParams.relaxationParamMin;
    fluidParams.splitsNum = problemParams.splitsNum;
    fluidParams.isRightSweepPedantic = problemParams.isRightSweepPedantic;

    return fluidParams;
}


MagneticParams getFieldParams(const ProblemParams& problemParams)
{
    MagneticParams fieldParams;

    fieldParams.chi = problemParams.chi;
    fieldParams.accuracy = problemParams.fieldAccuracy;
    fieldParams.gridParams = problemParams.gridParams;
    fieldParams.iterationsNumMax = problemParams.fieldIterationsMaxNum;
    fieldParams.relaxParamMin = problemParams.fieldRelaxParamMin;

    return fieldParams;
}

#pragma endregion


#pragma region Constructors

Solution::Solution(const ProblemParams& params) : mParams(params), 
                                                  mFluid(getFluidParams(params)), 
                                                  mField(getFieldParams(params)), 
                                                  mLastValidFluidSurface(mFluid.pointsNum()), 
                                                  mLastValidFieldGrid(mField.grid().parameters()), 
                                                  mLastValidFieldPotential(mField.grid().rowsNum(), mField.grid().columnsNum())
{
	if (params.resultsNum == 1)
	{
		mStepW = params.wTarget;
		mCurW = params.wTarget;
	}
	else
	{
		mStepW = params.wTarget / (params.resultsNum - 1);
		mCurW = 0.0;
	}
}

#pragma endregion


#pragma region Parameters

void Solution::setChi(double chi)
{
    mParams.chi = chi;
    mFluid.setChi(chi);
    mField.setChi(chi);
}


double Solution::currentW() const
{
    return mCurW;
}


double Solution::volumeNonDimMul() const
{
    return mFluid.volumeNondimMul();
}


double Solution::heightCoef() const
{
    return mFluid.heightCoef();
}


const Array<Vector2<double>>& Solution::fluidSurface() const
{
    return mFluid.lastValidResult();
}


const MagneticFluid& Solution::fluid() const
{
    return mFluid;
}


const MagneticField& Solution::field() const
{
    return mField;
}


void Solution::resetIterationsCounters()
{
    mFluid.resetIterationsCounter();
    mField.resetIterationsCounter();
}

#pragma endregion


#pragma region Main calculations

void Solution::calcInitials()
{
    mFluid.setRelaxationParam(1.0);
    mField.setRelaxationParam(1.0);

    mFluid.setW(mCurW);
    mFluid.calcInitialApproximation();
    mField.updateGrid(mFluid.lastValidResult());
    mField.calcInitialApproximation();
    mField.calcRelaxation();

    mFluid.setDerivatives(calcDerivatives());

    updateLastValidResults();
}


ResultCode Solution::calcResult(double w)
{
    ResultCode resultCode = INVALID_RESULT;

    mFluid.setW(w);

    while (resultCode != SUCCESS &&
           mFluid.currentRelaxationParam() >= mParams.relaxationParamMin &&
           mField.currentRelaxationParam() >= mParams.fieldRelaxParamMin)
    {
        resultCode = mFluid.calcRelaxation();

        if (resultCode == FLUID_SUCCESS)
        {
            mField.updateGrid(mFluid.lastValidResult());
            resultCode = mField.calcRelaxation();

            if (resultCode == FIELD_SUCCESS)
            {
                if (isAccuracyReached())
                {
                    resultCode = SUCCESS;
                }
                else
                {
                    resultCode = ACCURACY_NOT_REACHED;
                }

                updateLastValidResults();
                mFluid.setDerivatives(calcDerivatives());
            }
            else
            {
                mFluid.setLastValidResult(mLastValidFluidSurface);
                mField.setLastValidResult(mLastValidFieldPotential);
                mField.setGrid(mLastValidFieldGrid);

                mField.setRelaxationParam(0.5 * mField.currentRelaxationParam());
            }
        }
        else
        {
            mFluid.setLastValidResult(mLastValidFluidSurface);
            mField.setLastValidResult(mLastValidFieldPotential);
            mField.setGrid(mLastValidFieldGrid);

            mFluid.setRelaxationParam(0.5 * mFluid.currentRelaxationParam());
        }
    }

    if (resultCode == SUCCESS)
    {
        mCurW = w;

        if (mCurW >= mParams.wTarget || std::abs(mCurW - mParams.wTarget) <= 0.00001)
        {
            resultCode = TARGET_REACHED;
        }
    }
    else
    {
        mFluid.setW(mCurW);
    }

    return resultCode;
}


ResultCode Solution::calcNextResult()
{
    double nextW = mCurW + mStepW;
    return calcResult(nextW);
}

#pragma endregion


#pragma region Calculate derivatives

Array<Vector2<double>> Solution::calcDerivatives() const
{
    arr_size_t pointsNum = mFluid.pointsNum();
    arr_size_t limit = pointsNum - 1;
    Array<Vector2<double>> derivatives(pointsNum);
    double param = 0.0;

    for (arr_size_t i = 0; i < pointsNum; i++)
    {
        param = (double)i / limit;
        derivatives(i) = mField.calcInnerDerivative(param);
    }

    return derivatives;
}

#pragma endregion


#pragma region Update last valid result

void Solution::updateLastValidResults()
{
    mLastValidFluidSurface = mFluid.lastValidResult();
    mLastValidFieldGrid = mField.grid();
    mLastValidFieldPotential = mField.lastValidResult();
}

#pragma endregion


#pragma region Checks

bool Solution::isAccuracyReached() const
{
    return norm(mFluid.lastValidResult(), mLastValidFluidSurface) <= mParams.accuracy &&
           norm(mField.lastValidResult(), mLastValidFieldPotential) <= mParams.fieldAccuracy;
}

#pragma endregion
