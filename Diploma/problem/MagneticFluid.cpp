#include "MagneticFluid.h"
#include <algorithm>


MagneticFluid::MagneticFluid(const FluidParams& params)
{
	fluidParams = params;
	pointsNum = params.splitsNum + 1;
	step = 1.0 / params.splitsNum;
	iterationsCounter = 0U;
	relaxationParam = 1.0;

	rightSweep = new RightSweep(pointsNum);

	lastValidResult = new Vector2[pointsNum];
	derivatives = new Vector2[pointsNum];
	nextApproxR = new double[pointsNum];
	nextApproxZ = new double[pointsNum];
	curApproxR = new double[pointsNum];
	curApproxZ = new double[pointsNum];

	this->calcInitialApproximation();
}


MagneticFluid::~MagneticFluid()
{
	delete rightSweep;
	delete lastValidResult;
	delete derivatives;
	delete nextApproxR;
	delete nextApproxZ;
	delete curApproxR;
	delete curApproxZ;
}


#pragma mark Public methods

Vector2* MagneticFluid::getLastValidResult()
{
	return lastValidResult;
}


Vector2* MagneticFluid::getDerivatives()
{
	return derivatives;
}


FluidParams MagneticFluid::getFluidParams()
{
	return fluidParams;
}


ProblemResultCode MagneticFluid::getLastMagneticFieldResultCode()
{
	return lastFieldResultCode;
}


double MagneticFluid::getCurrentRelaxationParam()
{
	return relaxationParam;
}


double MagneticFluid::getCurrentW()
{
	return fluidParams.w;
}


int MagneticFluid::getPointsNum()
{
	return pointsNum;
}


unsigned int MagneticFluid::getIterationsCounter()
{
	return iterationsCounter;
}


void MagneticFluid::setLastValidResult(Vector2* result)
{
	for (int i = 0; i < pointsNum; i++)
	{
		lastValidResult[i] = result[i];
	}
}


void MagneticFluid::setW(double w)
{
	fluidParams.w = w;
}


void MagneticFluid::setChi(double chi)
{
	fluidParams.chi = chi;
}



void MagneticFluid::setRelaxationParam(double relaxParam)
{
	relaxationParam = relaxParam;
}


void MagneticFluid::setDerivatives(Vector2* derivs)
{
	for (int i = 0; i < pointsNum; i++)
	{
		derivatives[i] = derivs[i];
	}
}


double MagneticFluid::calcVolumeNondimMul()
{
	return 1.0 / cbrt(calcIntegralTrapeze(lastValidResult));
}


#pragma mark Private methods

void MagneticFluid::calcInitialApproximation()
{
	iterationsCounter = 0U;

	for (int i = 0; i < pointsNum; i++)
	{
		lastValidResult[i] = { M_2_PI * sin(M_PI_2 * i * step), M_2_PI * cos(M_PI_2 * i * step) };
	}
}


void MagneticFluid::calcNextApproximationR(const double* valZ, const double* prevValZ)
{
	double integralCbrt = cbrt(calcIntegralTrapeze(curApproxR, prevValZ));
	double integralCbrtSq = integralCbrt * integralCbrt;
	double valQ = calcQ(curApproxR, prevValZ, integralCbrt);
	double tmp = 0.0;

	rightSweep->mainDiagonal[0] = 1.0;
	rightSweep->upperDiagonal[0] = 0.0;

	rightSweep->mainDiagonal[1] = 1.0;
	rightSweep->upperDiagonal[1] = 0.0;
	rightSweep->lowerDiagonal[0] = 0.0;

	rightSweep->mainDiagonal[pointsNum - 1] = 1.0;
	rightSweep->lowerDiagonal[pointsNum - 2] = -1.0;

	rightSweep->vect[0] = 0.0;
	rightSweep->vect[1] = step;
	rightSweep->vect[pointsNum - 1] = 0.5 * step * step * 
									  (valQ - calcMagneticF(curApproxR, prevValZ, pointsNum - 1, integralCbrt) + 1.0 / curApproxR[pointsNum - 1]);

	int limit = pointsNum - 1;
	for (int i = 2; i < limit; i++)
	{
		rightSweep->lowerDiagonal[i - 1] = 1.0;
		rightSweep->mainDiagonal[i] = -2.0;
		rightSweep->upperDiagonal[i] = 1.0;

		tmp = 0.5 * (valZ[i + 1] - valZ[i - 1]);
		rightSweep->vect[i] = -step * tmp * (valQ - tmp / (step * curApproxR[i]) - calcMagneticF(curApproxR, prevValZ, i, integralCbrt));
	}

	rightSweep->calcRightSweep(nextApproxR);
}


void MagneticFluid::calcNextApproximationZ(const double* valR, const double* prevValR)
{
	double integralCbrt = cbrt(calcIntegralTrapeze(prevValR, curApproxZ));
	double integralCbrtSq = integralCbrt * integralCbrt;
	double valQ = calcQ(prevValR, curApproxZ, integralCbrt);
	double tmp = 0.25 * step * step;

	rightSweep->mainDiagonal[0] = -1.0;
	rightSweep->upperDiagonal[0] = 1.0;

	rightSweep->mainDiagonal[pointsNum - 2] = 1.0;
	rightSweep->upperDiagonal[pointsNum - 2] = 0.0;
	rightSweep->lowerDiagonal[pointsNum - 3] = 0.0;

	rightSweep->mainDiagonal[pointsNum - 1] = 1.0;
	rightSweep->lowerDiagonal[pointsNum - 2] = 0.0;

	rightSweep->vect[0] = tmp * (valQ - calcMagneticF(prevValR, curApproxZ, 0, integralCbrt));
	rightSweep->vect[pointsNum - 2] = step * (1.0 - 0.5 * step * (valQ - 
									  calcMagneticF(prevValR, curApproxZ, pointsNum - 1, integralCbrt) + 1.0 / prevValR[pointsNum - 1]));
	rightSweep->vect[pointsNum - 1] = 0.0;

	int limit = pointsNum - 2;
	for (int i = 1; i < limit; i++)
	{
		rightSweep->lowerDiagonal[i - 1] = 1.0 + valR[i - 1] / valR[i];
		rightSweep->mainDiagonal[i] = -(2.0 + (valR[i + 1] + valR[i - 1]) / valR[i]);
		rightSweep->upperDiagonal[i] = 1.0 + valR[i + 1] / valR[i];

		rightSweep->vect[i] = step * (valR[i + 1] - valR[i - 1]) * (valQ - calcMagneticF(prevValR, curApproxZ, i, integralCbrt));
	}

	rightSweep->calcRightSweep(nextApproxZ);
}


ProblemResultCode MagneticFluid::calcRelaxation()
{
	double* tmpPtr;
	double curEpsilon = fluidParams.epsilon * relaxationParam;
	int counter = 0;

	for (int i = 0; i < pointsNum; i++)
	{
		nextApproxR[i] = lastValidResult[i].r;
		nextApproxZ[i] = lastValidResult[i].z;
	}

	do
	{
		tmpPtr = nextApproxR;
		nextApproxR = curApproxR;
		curApproxR = tmpPtr;

		tmpPtr = nextApproxZ;
		nextApproxZ = curApproxZ;
		curApproxZ = tmpPtr;

		calcNextApproximationR(curApproxZ, curApproxZ);
		relaxation(nextApproxR, curApproxR, pointsNum, relaxationParam);

		calcNextApproximationZ(nextApproxR, curApproxR);
		relaxation(nextApproxZ, curApproxZ, pointsNum, relaxationParam);

		counter++;
	} while (std::max(norm(nextApproxR, curApproxR, pointsNum), norm(nextApproxZ, nextApproxZ, pointsNum)) > curEpsilon && 
				counter < fluidParams.iterationsNumMax);

	iterationsCounter += counter;

	if (counter >= fluidParams.iterationsNumMax)
	{
		return FLUID_ITERATIONS_LIMIT_EXCEEDED;
	}
	else if (!isApproximationValid(nextApproxR) || !isApproximationValid(nextApproxZ))
	{
		return FLUID_INVALID_RESULT;
	}
	else
	{
		for (int i = 0; i < pointsNum; i++)
		{
			lastValidResult[i].r = nextApproxR[i];
			lastValidResult[i].z = nextApproxZ[i];
		}
	}

	return FLUID_SUCCESS;
}


double MagneticFluid::calcIntegralTrapeze(const double* approxR, const double* approxZ)
{
	double result = 0.0;
	int limit = pointsNum - 1;

	for (int i = 1; i < limit; i++)
	{
		result += approxR[i] * approxZ[i] * (approxR[i + 1] - approxR[i - 1]);
	}

	return M_PI * result;
}


double MagneticFluid::calcIntegralTrapeze(const Vector2* approx)
{
	double result = 0.0;
	int limit = pointsNum - 1;

	for (int i = 1; i < limit; i++)
	{
		result += approx[i].r * approx[i].z * (approx[i + 1].r - approx[i - 1].r);
	}

	return M_PI * result;
}


double MagneticFluid::calcMagneticIntegralTrapeze(const double* approxR, const double* approxZ, double integralCbrt)
{
	double result = 0.0;
	int limit = pointsNum - 1;

	for (int i = 1; i < limit; i++)
	{
		result += approxR[i] * (approxR[i + 1] - approxR[i - 1]) * calcMagneticF(approxR, approxZ, i, integralCbrt);
	}

	return 0.5 * result;
}


double MagneticFluid::calcQ(const double* approxR, const double* approxZ, double integralCbrt)
{
	double value = 1.0 / approxR[pointsNum - 1];
	double magneticIntegral = calcMagneticIntegralTrapeze(approxR, approxZ, integralCbrt);
	return -2.0 * value * (1.0 - magneticIntegral * value);
}


double MagneticFluid::calcMagneticF(const double* approxR, const double* approxZ, int index, double integralCbrt)
{
	double tmp = 0.0;
	Vector2 deriv = derivatives[index];

	if (index == 0)
	{
		tmp = deriv.z;
	}
	else if (index == pointsNum - 1)
	{
		tmp = deriv.r;
	}
	else
	{
		tmp = 0.5 * (-(approxZ[index + 1] - approxZ[index - 1]) * deriv.r + 
					(approxR[index + 1] - approxR[index - 1]) * deriv.z) / step;
	}

	return 0.5 * fluidParams.w * (tmp * tmp + (deriv.r * deriv.r + deriv.z * deriv.z) / fluidParams.chi) / integralCbrt;
}


bool MagneticFluid::isApproximationValid(const double* approx)
{
	for (int i = 0; i < pointsNum; i++)
	{
		if (!std::isfinite(approx[i]) || approx[i] < -0.00001)
		{
			return false;
		}
	}

	return true;
}