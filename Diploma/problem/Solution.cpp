#include "Solution.h"
#include "math_ext.h"


static const std::string FIELD_MODEL_ACTION_KEY = "field-model";


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


#pragma region Actions

void Solution::setFluidActionForKey(const std::string& key, const MagneticFluidAction& action)
{
    mFluid.setActionForKey(key, action);
}


void Solution::removeFluidActionForKey(const std::string& key)
{
    mFluid.removeActionForKey(key);
}


void Solution::setFieldActionForKey(const std::string& key, const MagneticFieldAction& action)
{
    mField.setActionForKey(key, action);
}


void Solution::removeFieldActionForKey(const std::string& key)
{
    mField.removeActionForKey(key);
}


void Solution::fieldModelAction(const MagneticParams& params,
                                const Matrix<double>& nextApprox,
                                const Matrix<double>& curApprox,
                                const SimpleTriangleGrid& grid)
{
    double b = 3.0 / (3.0 + params.chi);
    double volume = M_PI * M_PI * M_PI;
    double discrepancy = 0.0;
    double maxDiscrepancy = 0.0;
    arr_size_t gridRowsNum = grid.rowsNum();
    arr_size_t gridColumnsNum = grid.columnsNum();
    arr_size_t gridSurfaceColumnIndex = grid.surfaceColumnsIndex();

    printf("\n=========================== FIELD DISCREPANCY ===========================\n");

    for (arr_size_t i = gridRowsNum - 1; i >= 0; i--)
    {
        for (arr_size_t j = 0; j < gridSurfaceColumnIndex; j++)
        {
            discrepancy = std::abs(nextApprox(i, j) - b * grid(i, j).z);
            maxDiscrepancy = std::max(maxDiscrepancy, discrepancy);
            printf("%.3e ", discrepancy);
        }

        discrepancy = std::abs(nextApprox(i, gridSurfaceColumnIndex) - b * grid(i, gridSurfaceColumnIndex).z);
        maxDiscrepancy = std::max(maxDiscrepancy, discrepancy);
        printf("| %.3e | ", discrepancy);

        for (arr_size_t j = gridSurfaceColumnIndex + 1; j < gridColumnsNum; j++)
        {
            double tmp = volume * std::pow(grid(i, j).r * grid(i, j).r + grid(i, j).z * grid(i, j).z, 1.5);
            discrepancy = std::abs(nextApprox(i, j) - grid(i, j).z * (1.0 - (1.0 - b) / tmp));
            maxDiscrepancy = std::max(maxDiscrepancy, discrepancy);
            printf("%.3e ", discrepancy);
        }

        printf("\n");
    }

    printf("\nMax discrepancy: %.3e \n\n", maxDiscrepancy);
}

#pragma endregion


#pragma region Main calculations

void Solution::calcInitials()
{
    mFluid.setRelaxationParam(mParams.relaxationParamInitial);
    mField.setRelaxationParam(mParams.fieldRelaxParamInitial);

    mFluid.setW(mCurW);
    mFluid.calcInitialApproximation();
    mField.updateGrid(mFluid.lastValidResult());
    mField.calcInitialApproximation();

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


ResultCode Solution::calcFieldModelProblem()
{
    ResultCode resultCode = INVALID_RESULT;

    mFluid.setRelaxationParam(mParams.relaxationParamInitial);
    mField.setRelaxationParam(mParams.fieldModelRelaxParamInitial);

    mField.setChi(mParams.fieldModelChi);

    mFluid.calcInitialApproximation();
    mField.updateGrid(mFluid.lastValidResult());
    mField.calcInitialApproximation();

    mField.setActionForKey(FIELD_MODEL_ACTION_KEY, std::bind(&Solution::fieldModelAction, this, 
                                                             std::placeholders::_1, std::placeholders::_2, 
                                                             std::placeholders::_3, std::placeholders::_4));

    while (resultCode != FIELD_SUCCESS && 
           mField.currentRelaxationParam() >= mParams.fieldModelRelaxParamMin)
    {
        resultCode = mField.calcRelaxation();

        if (resultCode != FIELD_SUCCESS)
        {
            mField.setRelaxationParam(0.5 * mFluid.currentRelaxationParam());
        }
    }

    mField.removeActionForKey(FIELD_MODEL_ACTION_KEY);

    return resultCode;
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
