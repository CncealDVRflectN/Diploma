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
                                                  mLastValidFieldPotential(mField.grid().rowsNum(), mField.grid().columnsNum()),
                                                  mLastFieldDiscrepancy(mField.grid().rowsNum(), mField.grid().columnsNum()),
                                                  mLastFieldDiscrepancyMin(std::numeric_limits<double>::max()),
                                                  mLastFieldDiscrepancyMax(std::numeric_limits<double>::min())
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


#pragma region Result parameters

FluidResultParams Solution::fluidResultParams() const
{
    FluidResultParams resultParams;

    resultParams.xLabel = mParams.xLabel;
    resultParams.yLabel = mParams.yLabel;
    resultParams.chi = mParams.chi;
    resultParams.w = currentW();
    resultParams.isDimensionless = mParams.isDimensionless;

    return resultParams;
}


FieldResultParams Solution::fieldResultParams() const
{
    arr_size_t rowsNum = mLastValidFieldGrid.rowsNum();
    arr_size_t surfaceColumnIndex = mField.grid().surfaceColumnsIndex();
    Vector2<double> limits = potentialLimits();

    FieldResultParams resultParams;

    resultParams.xLabel = mParams.xLabel;
    resultParams.yLabel = mParams.yLabel;
    resultParams.potentialLabel = mParams.potentialLabel;
    resultParams.potentialMin = limits.x;
    resultParams.potentialMax = limits.y;
    resultParams.fluidTopPotential = mLastValidFieldPotential(rowsNum - 1, surfaceColumnIndex);
    resultParams.chi = mParams.chi;
    resultParams.surfaceSplitsNum = mParams.gridParams.surfaceSplitsNum;
    resultParams.internalSplitsNum = mParams.gridParams.internalSplitsNum;
    resultParams.externalSplitsNum = mParams.gridParams.externalSplitsNum;
    resultParams.isDimensionless = mParams.isDimensionless;

    return resultParams;
}


FieldModelParams Solution::fieldModelParams() const
{
    FieldModelParams resultParams;

    resultParams.xLabel = mParams.xLabel;
    resultParams.yLabel = mParams.yLabel;
    resultParams.errorLabel = mParams.potentialLabel;
    resultParams.errorMin = mLastFieldDiscrepancyMin;
    resultParams.errorMax = mLastFieldDiscrepancyMax;
    resultParams.chi = mParams.fieldModelChi;
    resultParams.surfaceSplitsNum = mParams.gridParams.surfaceSplitsNum;
    resultParams.internalSplitsNum = mParams.gridParams.internalSplitsNum;
    resultParams.externalSplitsNum = mParams.gridParams.externalSplitsNum;
    resultParams.isDimensionless = mParams.isDimensionless;

    return resultParams;
}

#pragma endregion


#pragma region Writing data

std::filesystem::path Solution::writeFluidData() const
{
    double multiplier = (mParams.isDimensionless) ? volumeNonDimMul() : 1.0;
    return write_fluid_data(fluidResultParams(), multiplier * mLastValidFluidSurface);
}


void Solution::writeFluidData(const std::filesystem::path& fluidDataPath) const
{
    double multiplier = (mParams.isDimensionless) ? volumeNonDimMul() : 1.0;
    write_fluid_data(fluidDataPath, fluidResultParams(), multiplier * mLastValidFluidSurface);
}


std::filesystem::path Solution::writeFieldData() const
{
    double multiplier = (mParams.isDimensionless) ? volumeNonDimMul() : 1.0;
    return write_field_data(fieldResultParams(), multiplier * mLastValidFieldGrid.rawPoints(), mLastValidFieldPotential);
}


void Solution::writeFieldData(const std::filesystem::path& fieldDataPath) const
{
    double multiplier = (mParams.isDimensionless) ? volumeNonDimMul() : 1.0;
    write_field_data(fieldDataPath, fieldResultParams(), multiplier * mLastValidFieldGrid.rawPoints(), mLastValidFieldPotential);
}


std::filesystem::path Solution::writeFieldErrorData() const
{
    double multiplier = (mParams.isDimensionless) ? volumeNonDimMul() : 1.0;
    return write_field_error_data(fieldModelParams(), multiplier * mLastValidFieldGrid.rawPoints(), mLastFieldDiscrepancy);
}


void Solution::writeFieldErrorData(const std::filesystem::path& errorDataPath) const
{
    double multiplier = (mParams.isDimensionless) ? volumeNonDimMul() : 1.0;
    write_field_error_data(errorDataPath, fieldModelParams(), multiplier * mLastValidFieldGrid.rawPoints(), mLastFieldDiscrepancy);
}


std::filesystem::path Solution::writeInternalGridData() const
{
    double multiplier = (mParams.isDimensionless) ? volumeNonDimMul() : 1.0;
    return write_internal_grid_data(fieldResultParams(), multiplier * mLastValidFieldGrid.rawPoints());
}


void Solution::writeInternalGridData(const std::filesystem::path& gridDataPath) const
{
    double multiplier = (mParams.isDimensionless) ? volumeNonDimMul() : 1.0;
    write_internal_grid_data(gridDataPath, fieldResultParams(), multiplier * mLastValidFieldGrid.rawPoints());
}


std::filesystem::path Solution::writeExternalGridData() const
{
    double multiplier = (mParams.isDimensionless) ? volumeNonDimMul() : 1.0;
    return write_external_grid_data(fieldResultParams(), multiplier * mLastValidFieldGrid.rawPoints());
}


void Solution::writeExternalGridData(const std::filesystem::path& gridDataPath) const
{
    double multiplier = (mParams.isDimensionless) ? volumeNonDimMul() : 1.0;
    write_external_grid_data(gridDataPath, fieldResultParams(), multiplier * mLastValidFieldGrid.rawPoints());
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
    arr_size_t gridRowsNum = grid.rowsNum();
    arr_size_t gridColumnsNum = grid.columnsNum();
    arr_size_t gridSurfaceColumnIndex = grid.surfaceColumnsIndex();

    mLastFieldDiscrepancyMin = std::numeric_limits<double>::max();
    mLastFieldDiscrepancyMax = std::numeric_limits<double>::min();

    printf("\n=========================== FIELD DISCREPANCY ===========================\n");

    for (arr_size_t i = gridRowsNum - 1; i >= 0; i--)
    {
        for (arr_size_t j = 0; j < gridSurfaceColumnIndex; j++)
        {
            discrepancy = std::abs(nextApprox(i, j) - b * grid(i, j).z);
            mLastFieldDiscrepancy(i, j) = discrepancy;
            mLastFieldDiscrepancyMin = std::min(mLastFieldDiscrepancyMin, discrepancy);
            mLastFieldDiscrepancyMax = std::max(mLastFieldDiscrepancyMax, discrepancy);

            printf("%.3e ", discrepancy);
        }

        discrepancy = std::abs(nextApprox(i, gridSurfaceColumnIndex) - b * grid(i, gridSurfaceColumnIndex).z);
        mLastFieldDiscrepancyMin = std::min(mLastFieldDiscrepancyMin, discrepancy);
        mLastFieldDiscrepancyMax = std::max(mLastFieldDiscrepancyMax, discrepancy);
        mLastFieldDiscrepancy(i, gridSurfaceColumnIndex) = discrepancy;

        printf("| %.3e | ", discrepancy);

        for (arr_size_t j = gridSurfaceColumnIndex + 1; j < gridColumnsNum; j++)
        {
            double tmp = volume * std::pow(grid(i, j).r * grid(i, j).r + grid(i, j).z * grid(i, j).z, 1.5);

            discrepancy = std::abs(nextApprox(i, j) - grid(i, j).z * (1.0 - (1.0 - b) / tmp));
            mLastFieldDiscrepancyMin = std::min(mLastFieldDiscrepancyMin, discrepancy);
            mLastFieldDiscrepancyMax = std::max(mLastFieldDiscrepancyMax, discrepancy);
            mLastFieldDiscrepancy(i, j) = discrepancy;

            printf("%.3e ", discrepancy);
        }

        printf("\n");
    }

    printf("\nMax discrepancy: %.3e \n\n", mLastFieldDiscrepancyMax);
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
    ResultCode resultCode = ResultCode::INVALID_RESULT;

    mFluid.setW(w);

    while (resultCode != ResultCode::SUCCESS &&
           mFluid.currentRelaxationParam() >= mParams.relaxationParamMin &&
           mField.currentRelaxationParam() >= mParams.fieldRelaxParamMin)
    {
        resultCode = mFluid.calcRelaxation();

        if (resultCode == ResultCode::FLUID_SUCCESS)
        {
            mField.updateGrid(mFluid.lastValidResult());
            resultCode = mField.calcRelaxation();

            if (resultCode == ResultCode::FIELD_SUCCESS)
            {
                if (isAccuracyReached())
                {
                    resultCode = ResultCode::SUCCESS;
                }
                else
                {
                    resultCode = ResultCode::ACCURACY_NOT_REACHED;
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

    if (resultCode == ResultCode::SUCCESS)
    {
        mCurW = w;

        if (mCurW >= mParams.wTarget || std::abs(mCurW - mParams.wTarget) <= 0.00001)
        {
            resultCode = ResultCode::TARGET_REACHED;
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
    ResultCode resultCode = ResultCode::INVALID_RESULT;

    mFluid.setRelaxationParam(mParams.relaxationParamInitial);
    mField.setRelaxationParam(mParams.fieldModelRelaxParamInitial);

    mField.setChi(mParams.fieldModelChi);

    mFluid.calcInitialApproximation();
    mField.updateGrid(mFluid.lastValidResult());
    mField.calcInitialApproximation();

    mField.setActionForKey(FIELD_MODEL_ACTION_KEY, std::bind(&Solution::fieldModelAction, this, 
                                                             std::placeholders::_1, std::placeholders::_2, 
                                                             std::placeholders::_3, std::placeholders::_4));

    while (resultCode != ResultCode::FIELD_SUCCESS && 
           mField.currentRelaxationParam() >= mParams.fieldModelRelaxParamMin)
    {
        resultCode = mField.calcRelaxation();

        if (resultCode != ResultCode::FIELD_SUCCESS)
        {
            mField.setRelaxationParam(0.5 * mFluid.currentRelaxationParam());
        }
    }

    mField.removeActionForKey(FIELD_MODEL_ACTION_KEY);

    updateLastValidResults();

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


#pragma region Calculate limits

Vector2<double> Solution::potentialLimits() const
{
    Vector2<double> result(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());
    arr_size_t rowsNum = mLastValidFieldPotential.rowsNum();
    arr_size_t columnsNum = mLastValidFieldPotential.columnsNum();

    for (arr_size_t i = 0; i < rowsNum; i++)
    {
        for (arr_size_t j = 0; j < columnsNum; j++)
        {
            result.x = (mLastValidFieldPotential(i, j) < result.x) ? mLastValidFieldPotential(i, j) : result.x;
            result.y = (mLastValidFieldPotential(i, j) > result.y) ? mLastValidFieldPotential(i, j) : result.y;
        }
    }

    return result;
}

#pragma endregion


#pragma region Checks

bool Solution::isAccuracyReached() const
{
    return norm(mFluid.lastValidResult(), mLastValidFluidSurface) <= mParams.accuracy &&
           norm(mField.lastValidResult(), mLastValidFieldPotential) <= mParams.fieldAccuracy;
}

#pragma endregion
