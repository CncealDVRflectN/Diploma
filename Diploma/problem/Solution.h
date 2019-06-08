#ifndef DIPLOMA_SOLUTION_H
#define DIPLOMA_SOLUTION_H


#include "MagneticFluid.h"
#include "MagneticField.h"


typedef struct problem_params_t
{
	double wInitial;
	double wTarget;
	double relaxationParamMin;
	double fieldRelaxParamMin;
	double accuracy;
	double fieldAccuracy;
	double chi;
	int splitsNum;
	int fieldSurfaceSplitsNum;
	int fieldInternalSplitsNum;
	int fieldInfinitySplitsNum;
	int iterationsMaxNum;
	int fieldIterationsMaxNum;
	int resultsNum;
} ProblemParams;


class Solution
{
public:
	Solution(const ProblemParams& params);

	~Solution();

	MagneticFluid* getMagneticFluid();

	MagneticField* getMagneticField();

	Vector2* getLastValidSurface();

	Vector2** getLastValidFieldGrid();

	double** getLastValidFieldPotential();

	double getCurrentW();

	int getSurfacePointsNum();

	int getGridLinesNum();

	int getGridColumnsNum();

	int getGridSurfaceColumnIndex();

	void setChi(double chi);

	void calcInitials();

	ProblemResultCode calcResult(double w);

	ProblemResultCode calcNextResult();

private:
	MagneticField* field;
	MagneticFluid* fluid;

	ProblemParams params;

	Vector2* lastValidFluidSurface;
	Vector2** lastValidFieldGrid;
	double** lastValidFieldPotential;

	double curW;
	double stepW;


	FluidParams getFluidParams(const ProblemParams& problemParams);

	MagneticParams getFieldParams(const ProblemParams& problemParams);

	void updateLastValidResults();

	void calcDerivatives();

	bool isAccuracyReached();
};


#endif