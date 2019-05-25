#ifndef DIPLOMA_RIGHTSWEEP_H
#define DIPLOMA_RIGHTSWEEP_H

#include "util.h"

class RightSweep
{
public:
	double* lowerDiagonal;
	double* mainDiagonal;
	double* upperDiagonal;
	double* vect;

	RightSweep(int size);

	~RightSweep();

	void calcRightSweep(double* solutionDest);

private:
	double* alpha;
	double* beta;

	int size;


	void calcAlpha();

	void calcBeta();

	void calcSolution(double* solutionDest);

	bool isValid();
};


#endif //DIPLOMA_RIGHTSWEEP_H