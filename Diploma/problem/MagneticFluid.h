#ifndef DIPLOMA_MAGNETICFLUID_H
#define DIPLOMA_MAGNETICFLUID_H

#include "../util/util.h"
#include "../util/RightSweep.h"
#include "MagneticField.h"


enum FluidResultCode
{
	FLUID_SUCCESS,
	FLUID_ITERATIONS_LIMIT_EXCEEDED,
	FLUID_INVALID_RESULT,
	FLUID_TARGET_BOND_REACHED,
	FLUID_TARGET_BOND_NOT_REACHABLE
};


typedef struct fluid_params_t
{
	double alpha;
	double bond;
	double bondStep;
	double relaxParamMin;
	double epsilon;
	double chi;
	double w;
	int splitsNum;
	int iterationsNumMax;
} FluidParams;


class MagneticFluid
{
public:
	MagneticFluid(const FluidParams& params, const MagneticParams& magneticParams);

	~MagneticFluid();

	const Vector2* getLastValidResult();

	FluidParams getFluidParams();

	MagneticField* getMagneticField();

	MagneticFieldResultCode getLastMagneticFieldResultCode();

	double getCurrentRelaxParam();

	double getCurrentBond();

	int getPointsNum();

	unsigned int getIterationsCounter();

	FluidResultCode calcNextValidResult();

	double calcVolumeNondimMul();

private:
	FluidParams fluidParams;

	RightSweep* rightSweep;

	MagneticField* magneticField;

	MagneticFieldResultCode lastFieldResultCode;

	Vector2* lastValidResult;
	double* nextApproxR;
	double* nextApproxZ;
	double* curApproxR;
	double* curApproxZ;

	double relaxationParam;
	double curBond;
	double nextBond;
	double step;

	int pointsNum;

	unsigned int iterationsCounter;


	void calcInitialApproximation();

	void calcNextApproximationR(double bond, const double* valZ, const double* prevValZ);

	void calcNextApproximationZ(double bond, const double* valR, const double* prevValR);

	FluidResultCode calcRelaxation();

	double calcIntegralTrapeze(const double* approxR, const double* approxZ);

	double calcIntegralTrapeze(const Vector2* approx);

	double calcMagneticIntegralTrapeze(const double* approxR, const double* approxZ, double integralCbrt);

	double calcQ(const double* approxR, const double* approxZ, double bond, double integralCbrt);

	double calcMagneticF(const double* approxR, const double* approxZ, int index, double integralCbrt);

	bool isApproximationValid(const double* approx);
};


#endif