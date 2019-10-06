#ifndef DIPLOMA_SOLUTION_H
#define DIPLOMA_SOLUTION_H


#include "MagneticFluid.h"
#include "MagneticField.h"


typedef struct problem_params_t
{
    STGridParams gridParams;
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
};


#endif