#include "RightSweep.h"
#include <cmath>
#include <iostream>


RightSweep::RightSweep(int size)
{
	lowerDiagonal = new double[size - 1];
	mainDiagonal = new double[size];
	upperDiagonal = new double[size - 1];
	vect = new double[size];

	alpha = new double[size - 1];
	beta = new double[size];

	this->size = size;
}


RightSweep::~RightSweep()
{
	delete lowerDiagonal;
	delete mainDiagonal;
	delete upperDiagonal;
	delete vect;

	delete alpha;
	delete beta;
}


#pragma mark Public methods

void RightSweep::calcRightSweep(double* solutionDest)
{
	if (!isValid())
	{
		std::cerr << "Warning: right sweep matrix is invalid!" << std::endl;
	}

	calcAlpha();
	calcBeta();

	calcSolution(solutionDest);
}


#pragma mark Private methods

void RightSweep::calcAlpha()
{
	int limit = size - 1;

	alpha[0] = -upperDiagonal[0] / mainDiagonal[0];
	for (int i = 1; i < limit; i++)
	{
		alpha[i] = -upperDiagonal[i] / (mainDiagonal[i] + lowerDiagonal[i - 1] * alpha[i - 1]);
	}
}


void RightSweep::calcBeta()
{
	beta[0] = vect[0] / mainDiagonal[0];

	for (int i = 1; i < size; i++)
	{
		beta[i] = (vect[i] - lowerDiagonal[i - 1] * beta[i - 1]) /
			(mainDiagonal[i] + lowerDiagonal[i - 1] * alpha[i - 1]);
	}
}


void RightSweep::calcSolution(double* solutionDest)
{
	solutionDest[size - 1] = beta[size - 1];
	for (int i = size - 2; i >= 0; i--)
	{
		solutionDest[i] = alpha[i] * solutionDest[i + 1] + beta[i];
	}
}


bool RightSweep::isValid()
{
	if (fabs(mainDiagonal[0]) < fabs(upperDiagonal[0]))
	{
		return false;
	}

	if (fabs(mainDiagonal[size - 1]) < fabs(lowerDiagonal[size - 2]))
	{
		return false;
	}

	int lowerDiagSize = size - 1;
	for (int i = 1; i < lowerDiagSize; i++)
	{
		if (fabs(mainDiagonal[i]) < (fabs(lowerDiagonal[i - 1]) + fabs(upperDiagonal[i])))
		{
			return false;
		}
	}

	return true;
}