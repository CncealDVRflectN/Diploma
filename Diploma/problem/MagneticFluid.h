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
	double w;
	int splitsNum;
	int iterationsNumMax;
} FluidParams;


class MagneticFluid
{
//public:
//	MagneticFluid(const FluidParams& params);
//
//	~MagneticFluid();
//
//	Vector2* getLastValidResult();
//
//	Vector2* getDerivatives();
//
//	FluidParams getFluidParams();
//
//	ProblemResultCode getLastMagneticFieldResultCode();
//
//	double getCurrentRelaxationParam();
//
//	double getCurrentW();
//
//	int getPointsNum();
//
//	unsigned int getIterationsCounter();
//
//	void setLastValidResult(Vector2* result);
//
//	void setW(double w);
//
//	void setChi(double chi);
//
//	void setRelaxationParam(double relaxParam);
//
//	void setDerivatives(Vector2* derivs);
//
//	void calcInitialApproximation();
//
//	ProblemResultCode calcRelaxation();
//
//	double calcVolumeNondimMul();
//
//private:
//	FluidParams fluidParams;
//
//	RightSweep* rightSweep;
//
//	ProblemResultCode lastFieldResultCode;
//
//	Vector2* lastValidResult;
//	Vector2* derivatives;
//	double* nextApproxR;
//	double* nextApproxZ;
//	double* curApproxR;
//	double* curApproxZ;
//
//	double relaxationParam;
//	double step;
//
//	int pointsNum;
//
//	unsigned int iterationsCounter;
//
//
//	void calcNextApproximationR(const double* valZ, const double* prevValZ);
//
//	void calcNextApproximationZ(const double* valR, const double* prevValR);
//
//	double calcIntegralTrapeze(const double* approxR, const double* approxZ);
//
//	double calcIntegralTrapeze(const Vector2* approx);
//
//	double calcMagneticIntegralTrapeze(const double* approxR, const double* approxZ, double integralCbrt);
//
//	double calcQ(const double* approxR, const double* approxZ, double integralCbrt);
//
//	double calcMagneticF(const double* approxR, const double* approxZ, int index, double integralCbrt);
//
//	bool isApproximationValid(const double* approx);
};


#endif