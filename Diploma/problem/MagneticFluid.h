#ifndef DIPLOMA_MAGNETICFLUID_H
#define DIPLOMA_MAGNETICFLUID_H

#include "../util/util.h"
#include "../util/RightSweep.h"
#include "MagneticField.h"


typedef struct fluid_params_t
{
	double relaxParamMin;
	double epsilon;
	double chi;
	int splitsNum;
	int iterationsNumMax;
} FluidParams;


class MagneticFluid
{
public:
	double w;


	MagneticFluid(const FluidParams& params, const MagneticParams& magneticParams);

	~MagneticFluid();

	const Vector2* getLastValidResult();

	FluidParams getFluidParams();

	MagneticField* getMagneticField();

	ProblemResultCode getLastMagneticFieldResultCode();

	double getCurrentRelaxParam();

	int getPointsNum();

	unsigned int getIterationsCounter();

	ProblemResultCode calcResult();

	double calcVolumeNondimMul();

private:
	FluidParams fluidParams;

	RightSweep* rightSweep;

	MagneticField* magneticField;

	ProblemResultCode lastFieldResultCode;

	Vector2* lastValidResult;
	double* nextApproxR;
	double* nextApproxZ;
	double* curApproxR;
	double* curApproxZ;

	double relaxationParam;
	double step;

	int pointsNum;

	unsigned int iterationsCounter;


	void calcInitialApproximation();

	void calcNextApproximationR(const double* valZ, const double* prevValZ);

	void calcNextApproximationZ(const double* valR, const double* prevValR);

	ProblemResultCode calcRelaxation();

	double calcIntegralTrapeze(const double* approxR, const double* approxZ);

	double calcIntegralTrapeze(const Vector2* approx);

	double calcMagneticIntegralTrapeze(const double* approxR, const double* approxZ, double integralCbrt);

	double calcQ(const double* approxR, const double* approxZ, double integralCbrt);

	double calcMagneticF(const double* approxR, const double* approxZ, int index, double integralCbrt);

	bool isApproximationValid(const double* approx);
};


#endif