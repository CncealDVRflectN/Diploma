#include "MagneticFluid.h"
#include <algorithm>


MagneticFluid::MagneticFluid(const FluidParams& params, const MagneticParams& magneticParams)
{
	fluidParams = params;
	pointsNum = params.splitsNum + 1;
	step = 1.0 / params.splitsNum;
	iterationsCounter = 0U;
	curBond = 0.0;
	nextBond = 0.0;
	relaxationParam = 1.0;

	rightSweep = new RightSweep(pointsNum);

	lastValidResult = new Vector2[pointsNum];
	nextApproxR = new double[pointsNum];
	nextApproxZ = new double[pointsNum];
	curApproxR = new double[pointsNum];
	curApproxZ = new double[pointsNum];

	magneticField = new MagneticField(magneticParams);

	this->calcInitialApproximation();
}


MagneticFluid::~MagneticFluid()
{
	delete rightSweep;
	delete lastValidResult;
	delete nextApproxR;
	delete nextApproxZ;
	delete curApproxR;
	delete curApproxZ;

	delete magneticField;
}


#pragma mark Public methods

const Vector2* MagneticFluid::getLastValidResult()
{
	return lastValidResult;
}


FluidParams MagneticFluid::getFluidParams()
{
	return fluidParams;
}


MagneticField* MagneticFluid::getMagneticField()
{
	return magneticField;
}


MagneticFieldResultCode MagneticFluid::getLastMagneticFieldResultCode()
{
	return lastFieldResultCode;
}


double MagneticFluid::getCurrentRelaxParam()
{
	return relaxationParam;
}


double MagneticFluid::getCurrentBond()
{
	return curBond;
}


int MagneticFluid::getPointsNum()
{
	return pointsNum;
}


unsigned int MagneticFluid::getIterationsCounter()
{
	return iterationsCounter;
}


FluidResultCode MagneticFluid::calcNextValidResult()
{
	FluidResultCode resultCode = FLUID_INVALID_RESULT;

	if (curBond >= fluidParams.bond && iterationsCounter != 0)
	{
		return FLUID_TARGET_BOND_REACHED;
	}

	while (relaxationParam >= fluidParams.relaxParamMin && resultCode != FLUID_SUCCESS && resultCode != FLUID_TARGET_BOND_REACHED)
	{
		resultCode = calcRelaxation();

		if (resultCode != FLUID_SUCCESS && resultCode != FLUID_TARGET_BOND_REACHED)
		{
			calcInitialApproximation();
			relaxationParam *= 0.5;
		}
	}

	if (relaxationParam < fluidParams.relaxParamMin)
	{
		return FLUID_TARGET_BOND_NOT_REACHABLE;
	}

	return resultCode;
}


double MagneticFluid::calcVolumeNondimMul()
{
	return 1.0 / cbrt(calcIntegralTrapeze(lastValidResult));
}


#pragma mark Private methods

void MagneticFluid::calcInitialApproximation()
{
	curBond = 0.0;
	iterationsCounter = 0U;

	for (int i = 0; i < pointsNum; i++)
	{
		lastValidResult[i] = { sin(fluidParams.alpha * i * step) / fluidParams.alpha, 
							   (cos(fluidParams.alpha * i * step) - cos(fluidParams.alpha)) / fluidParams.alpha };
	}

	magneticField->generateGrid(lastValidResult, pointsNum);
	magneticField->calcResult();
}


void MagneticFluid::calcNextApproximationR(double bond, const double* valZ, const double* prevValZ)
{
	double integralCbrt = cbrt(calcIntegralTrapeze(curApproxR, prevValZ));
	double integralCbrtSq = integralCbrt * integralCbrt;
	double valQ = calcQ(curApproxR, prevValZ, bond, integralCbrt);
	double tmp = sin(fluidParams.alpha);

	rightSweep->mainDiagonal[0] = 1.0;
	rightSweep->upperDiagonal[0] = 0.0;
	rightSweep->lowerDiagonal[pointsNum - 2] = -1.0;
	rightSweep->mainDiagonal[pointsNum - 1] = 1.0;

	rightSweep->vect[0] = 0.0;
	rightSweep->vect[pointsNum - 1] = step * (cos(fluidParams.alpha) -
										0.5 * step * tmp * (tmp / curApproxR[pointsNum - 1] + valQ - 
											calcMagneticF(curApproxR, prevValZ, pointsNum - 1, integralCbrt)));

	int splitsNum = pointsNum - 1;
	for (int i = 1; i < splitsNum; i++)
	{
		rightSweep->lowerDiagonal[i - 1] = 1.0;
		rightSweep->mainDiagonal[i] = -2.0;
		rightSweep->upperDiagonal[i] = 1.0;

		tmp = 0.5 * (valZ[i + 1] - valZ[i - 1]);
		rightSweep->vect[i] = -step * tmp * (bond * prevValZ[i] / integralCbrtSq - tmp / (step * curApproxR[i]) - 
								calcMagneticF(curApproxR, prevValZ, i, integralCbrt) + valQ);
	}

	rightSweep->calcRightSweep(nextApproxR);
}


void MagneticFluid::calcNextApproximationZ(double bond, const double* valR, const double* prevValR)
{
	double integralCbrt = cbrt(calcIntegralTrapeze(prevValR, curApproxZ));
	double integralCbrtSq = integralCbrt * integralCbrt;
	double valQ = calcQ(prevValR, curApproxZ, bond, integralCbrt);
	double tmp = 0.25 * step * step;

	rightSweep->mainDiagonal[0] = -(1.0 + tmp * bond / integralCbrtSq);
	rightSweep->upperDiagonal[0] = 1.0;
	rightSweep->lowerDiagonal[pointsNum - 2] = 0.0;
	rightSweep->mainDiagonal[pointsNum - 1] = 1.0;

	rightSweep->vect[0] = tmp * (valQ - calcMagneticF(prevValR, curApproxZ, 0, integralCbrt));
	rightSweep->vect[pointsNum - 1] = 0.0;

	int splitsNum = pointsNum - 1;
	for (int i = 1; i < splitsNum; i++)
	{
		rightSweep->lowerDiagonal[i - 1] = 1.0 + valR[i - 1] / valR[i];
		rightSweep->mainDiagonal[i] = -(2.0 + (valR[i + 1] + valR[i - 1]) / valR[i]);
		rightSweep->upperDiagonal[i] = 1.0 + valR[i + 1] / valR[i];

		rightSweep->vect[i] = step * (valR[i + 1] - valR[i - 1]) * (bond * curApproxZ[i] / integralCbrtSq - 
								calcMagneticF(prevValR, curApproxZ, i, integralCbrt) + valQ);
	}

	rightSweep->calcRightSweep(nextApproxZ);
}


FluidResultCode MagneticFluid::calcRelaxation()
{
	MagneticFieldResultCode fieldResultCode = FIELD_INVALID_RESULT;
	double* tmpPtr;
	double curEpsilon = fluidParams.epsilon * relaxationParam;
	unsigned int counter = 0U;

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

		calcNextApproximationR(nextBond, curApproxZ, curApproxZ);
		relaxation(nextApproxR, curApproxR, pointsNum, relaxationParam);

		calcNextApproximationZ(nextBond, nextApproxR, curApproxR);
		relaxation(nextApproxZ, curApproxZ, pointsNum, relaxationParam);

		magneticField->generateGrid(nextApproxR, nextApproxZ, pointsNum);
		fieldResultCode = magneticField->calcResult();

		counter++;
	} while (std::max(norm(nextApproxR, curApproxR, pointsNum), norm(nextApproxZ, nextApproxZ, pointsNum)) > curEpsilon && 
				counter < fluidParams.iterationsNumMax && fieldResultCode == FIELD_SUCCESS);

	iterationsCounter += counter;

	if (counter >= fluidParams.iterationsNumMax)
	{
		return FLUID_ITERATIONS_LIMIT_EXCEEDED;
	}
	else if (!isApproximationValid(nextApproxR) || !isApproximationValid(nextApproxZ) || fieldResultCode != FIELD_SUCCESS)
	{
		if (fieldResultCode != FIELD_SUCCESS)
		{
			magneticField->calcInitialApproximation();
		}

		return FLUID_INVALID_RESULT;
	}
	else
	{
		curBond = nextBond;
		nextBond = std::min(curBond + fluidParams.bondStep, fluidParams.bond);

		for (int i = 0; i < pointsNum; i++)
		{
			lastValidResult[i].r = nextApproxR[i];
			lastValidResult[i].z = nextApproxZ[i];
		}
	}

	if (curBond == fluidParams.bond)
	{
		return FLUID_TARGET_BOND_REACHED;
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


double MagneticFluid::calcQ(const double* approxR, const double* approxZ, double bond, double integralCbrt)
{
	double value = approxR[pointsNum - 1];
	double magneticIntegral = calcMagneticIntegralTrapeze(approxR, approxZ, integralCbrt);
	return -(2.0 * (sin(fluidParams.alpha) - magneticIntegral) + bond * integralCbrt / (M_PI * value)) / value;
}


double MagneticFluid::calcMagneticF(const double* approxR, const double* approxZ, int index, double integralCbrt)
{
	double tmp = 0.0;
	double param = index / (pointsNum - 1);
	Vector2 deriv = magneticField->calcInnerDerivative(param);

	if (index == 0)
	{
		tmp = (-(approxZ[index + 1] - approxZ[index]) * deriv.r +
				(approxR[index + 1] - approxR[index]) * deriv.z) / step;
	}
	else if (index == pointsNum - 1)
	{
		tmp = (-(approxZ[index] - approxZ[index - 1]) * deriv.r +
				(approxR[index] - approxR[index - 1]) * deriv.z) / step;
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
		if (!std::isfinite(approx[i]))
		{
			return false;
		}
	}

	return true;
}