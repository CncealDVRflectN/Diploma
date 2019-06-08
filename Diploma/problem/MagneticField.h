#ifndef DIPLOMA_MAGNETICFIELD_H
#define DIPLOMA_MAGNETICFIELD_H

#include "../util/util.h"


typedef struct magnetic_params_t
{
	int surfaceSplitsNum;
	int infSplitsNum;
	int internalSplitsNum;
	int iterationsNumMax;
	double chi;
	double accuracy;
	double relaxParamMin;
} MagneticParams;


class MagneticField
{
public:
	MagneticField(const MagneticParams& params);

	~MagneticField();

	Vector2** getGrid();

	double** getLastValidResult();

	Vector2* getInnerDerivatives();

	Vector2* getOuterDerivatives();

	int getGridLinesNum();

	int getGridColumnsNum();

	int getSurfaceColumnIndex();

	unsigned int getIterationsCounter();

	double getCurrentRelaxationParam();

	void setLastValidResult(double** result);

	void setGrid(Vector2** grid);

	void setRelaxationParam(double relaxParam);

	void setChi(double chi);

	ProblemResultCode calcRelaxation();

	Vector2 calcInnerDerivative(double param);

	Vector2 calcOuterDerivative(double param);

	void generateGrid(const double* surfacePointsR, const double* surfacePointsZ, int pointsNum);

	void generateGrid(const Vector2* surfacePoints, int pointsNum);

	void calcInitialApproximation();
private:
	MagneticParams params;

	Vector2** grid;

	double** lastValidValues;
	double** curApprox;
	double** nextApprox;

	Vector2* innerDerivatives;
	Vector2* outerDerivatives;

	double relaxationParam;

	int gridLinesNum;
	int gridColumnsNum;
	int surfaceColumnIndex;

	unsigned int iterationsCounter;


	double calcCoefficientIntegral(const Vector2& vert1, const Vector2& vert2, const Vector2& vert3, double doubleTriangleArea);

	double calcCoefficient(int i, int j, int coefIndex);

	double calcNextValue(int i, int j);

	void calcNextApproximation();

	bool isApproximationValid(double** approx);

	bool isIndicesValid(const Indices2& indices);

	void calcDerivatives();
};

#endif