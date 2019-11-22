#ifndef DIPLOMA_SOLUTION_H
#define DIPLOMA_SOLUTION_H


#include "MagneticFluid.h"
#include "MagneticField.h"
#include "files_util.h"


typedef struct problem_params_t
{
    STGridParams gridParams;
    std::string xLabel;
    std::string yLabel;
    std::string potentialLabel;
    double wTarget;
    double relaxationParamInitial;
    double relaxationParamMin;
    double fieldRelaxParamInitial;
    double fieldRelaxParamMin;
    double fieldModelRelaxParamInitial;
    double fieldModelRelaxParamMin;
    double accuracy;
    double fieldAccuracy;
    double chi;
    double fieldModelChi;
    int splitsNum;
    int iterationsMaxNum;
    int fieldIterationsMaxNum;
    int resultsNum;
    bool isRightSweepPedantic;
    bool isDimensionless;
} ProblemParams;


class Solution
{
public:
    Solution(const ProblemParams& params);


    void setChi(double chi);

    double currentW() const;

    double volumeNonDimMul() const;

    double heightCoef() const;

    const Array<Vector2<double>>& fluidSurface() const;

    const MagneticFluid& fluid() const;

    const MagneticField& field() const;

    void resetIterationsCounters();


    FluidResultParams fluidResultParams() const;

    FieldResultParams fieldResultParams() const;


    std::filesystem::path writeFluidData() const;

    void writeFluidData(const std::filesystem::path& fluidDataPath) const;

    std::filesystem::path writeFieldData() const;

    void writeFieldData(const std::filesystem::path& fieldDataPath) const;

    std::filesystem::path writeInternalGridData() const;

    void writeInternalGridData(const std::filesystem::path& gridDataPath) const;

    std::filesystem::path writeExternalGridData() const;

    void writeExternalGridData(const std::filesystem::path& gridDataPath) const;


    void setFluidActionForKey(const std::string& key, const MagneticFluidAction& action);

    void removeFluidActionForKey(const std::string& key);

    void setFieldActionForKey(const std::string& key, const MagneticFieldAction& action);

    void removeFieldActionForKey(const std::string& key);
    
    
    void calcInitials();
    
    ResultCode calcResult(double w);
    
    ResultCode calcNextResult();
    
    ResultCode calcFieldModelProblem();

private:
    ProblemParams mParams;
    
    MagneticField mField;
    MagneticFluid mFluid;
    
    SimpleTriangleGrid mLastValidFieldGrid;
    Array<Vector2<double>> mLastValidFluidSurface;
    Matrix<double> mLastValidFieldPotential;
    
    double mCurW;
    double mStepW;
    
    
    void updateLastValidResults();
    
    Array<Vector2<double>> calcDerivatives() const;
    
    void fieldModelAction(const MagneticParams& params,
                          const Matrix<double>& nextApprox,
                          const Matrix<double>& curApprox,
                          const SimpleTriangleGrid& grid);
    
    bool isAccuracyReached() const;
    
    Vector2<double> potentialLimits() const;
};


#endif