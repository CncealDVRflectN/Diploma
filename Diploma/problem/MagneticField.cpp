#include "MagneticField.h"

#include <iostream>
#include <vector>


static const Indices2 TABLE_OFFSETS[8] = { {-1,  1}, 
										   { 0,  1}, 
										   { 1,  0}, 
										   { 1, -1}, 
										   { 0, -1}, 
										   {-1,  0}, 
										   {-1,  1}, 
										   { 0,  1} };


MagneticField::MagneticField(const MagneticParams& params)
{
	this->params = params;

	gridLinesNum = params.surfaceSplitsNum + 1;
	gridColumnsNum = params.internalSplitsNum + params.infSplitsNum + 1;
	surfaceColumnIndex = params.internalSplitsNum;

	relaxationParam = 1.0;

	lastValidGrid = new Vector2*[gridLinesNum];
	grid = new Vector2*[gridLinesNum];
	lastValidValues = new double*[gridLinesNum];
	nextApprox = new double*[gridLinesNum];
	curApprox = new double*[gridLinesNum];

	for (int i = 0; i < gridLinesNum; i++)
	{
		lastValidGrid[i] = new Vector2[gridColumnsNum];
		grid[i] = new Vector2[gridColumnsNum];
		lastValidValues[i] = new double[gridColumnsNum];
		nextApprox[i] = new double[gridColumnsNum];
		curApprox[i] = new double[gridColumnsNum];
	}

	innerDerivatives = new Vector2[params.surfaceSplitsNum + 1];
	outerDerivatives = new Vector2[params.surfaceSplitsNum + 1];

	iterationsCounter = 0U;

	this->calcInitialApproximation();
}


MagneticField::~MagneticField()
{
	for (int i = 0; i < gridLinesNum; i++)
	{
		delete lastValidGrid[i];
		delete grid[i];
		delete lastValidValues[i];
		delete nextApprox[i];
		delete curApprox[i];
	}

	delete lastValidGrid;
	delete grid;
	delete lastValidValues;
	delete nextApprox;
	delete curApprox;

	delete innerDerivatives;
	delete outerDerivatives;
}


#pragma mark Public methods


Vector2** MagneticField::getLastValidGrid()
{
	return lastValidGrid;
}


double** MagneticField::getLastValidResult()
{
	return lastValidValues;
}


int MagneticField::getGridLinesNum()
{
	return gridLinesNum;
}


int MagneticField::getGridColumnsNum()
{
	return gridColumnsNum;
}


unsigned int MagneticField::getIterationsCounter()
{
	return iterationsCounter;
}


MagneticFieldResultCode MagneticField::calcResult()
{
	MagneticFieldResultCode resultCode = FIELD_INVALID_RESULT;

	while (relaxationParam >= params.relaxParamMin && resultCode != FIELD_SUCCESS)
	{
		resultCode = calcRelaxation();

		if (resultCode != FIELD_SUCCESS)
		{
			relaxationParam *= 0.5;
		}
	}

	return resultCode;
}


Vector2 MagneticField::calcInnerDerivative(double param)
{
	return calcParametricPoint(innerDerivatives, params.surfaceSplitsNum + 1, param);
}


Vector2 MagneticField::calcOuterDerivative(double param)
{
	return calcParametricPoint(outerDerivatives, params.surfaceSplitsNum + 1, param);
}


void MagneticField::generateGrid(const double* surfacePointsR, const double* surfacePointsZ, int pointsNum)
{
	std::vector<Vector2> surfacePoint(pointsNum);

	for (int i = 0; i < pointsNum; i++)
	{
		surfacePoint[i].r = surfacePointsR[i];
		surfacePoint[i].z = surfacePointsZ[i];
	}

	generateGrid(surfacePoint.data(), pointsNum);
}


void MagneticField::generateGrid(const Vector2* surfacePoints, int pointsNum)
{
	double topPointZ = calcParametricPoint(surfacePoints, pointsNum, 0.0).z;
	double symmetryStep = topPointZ / (params.internalSplitsNum + params.surfaceSplitsNum);
	double specialPointZ = symmetryStep * params.surfaceSplitsNum;
	double param = 0.0;
	Vector2 surfacePoint;

	int limitLines = gridLinesNum - 1;
	int limitColumns = gridColumnsNum - 1;

	for (int i = 0; i < gridLinesNum; i++)
	{
		param = (double)i / limitLines;
		surfacePoint = calcParametricPoint(surfacePoints, pointsNum, (1.0 - param));

		grid[i][0] = { 0.0, specialPointZ * param };
		grid[i][surfaceColumnIndex] = surfacePoint;
		grid[i][limitColumns] = { 4.0 * surfacePoint.r, 4.0 * surfacePoint.z };
	}

	double intersectionPointR = calcParametricPoint(surfacePoints, pointsNum, 1.0).r;

	for (int i = 1; i < surfaceColumnIndex; i++)
	{
		param = (double)i / surfaceColumnIndex;

		grid[0][i] = { intersectionPointR * param, 0.0 };
		grid[limitLines][i] = { 0.0, calcLerp(specialPointZ, topPointZ, param) };
	}

	double infTopPointZ = 4.0 * topPointZ;
	double infIntersectionPointR = 4.0 * intersectionPointR;

	for (int i = surfaceColumnIndex + 1; i < gridColumnsNum; i++)
	{
		param = (double)(i - surfaceColumnIndex) / (limitColumns - surfaceColumnIndex);

		grid[0][i] = { calcLerp(intersectionPointR, infIntersectionPointR, param), 0.0 };
		grid[limitLines][i] = { 0.0, calcLerp(topPointZ, infTopPointZ, param) };
	}

	for (int i = 1; i < limitLines; i++)
	{
		for (int j = 1; j < surfaceColumnIndex; j++)
		{
			param = (double)j / surfaceColumnIndex;

			grid[i][j] = calcLerp(grid[i][0], grid[i][surfaceColumnIndex], param);
		}

		for (int j = surfaceColumnIndex + 1; j < limitColumns; j++)
		{
			param = (double)(j - surfaceColumnIndex) / (limitColumns - surfaceColumnIndex);

			grid[i][j] = calcLerp(grid[i][surfaceColumnIndex], grid[i][limitColumns], param);
		}
	}

	for (int i = 0; i < gridLinesNum; i++)
	{
		lastValidValues[i][limitColumns] = grid[i][limitColumns].z;
	}
}


#pragma mark Private methods

void MagneticField::calcInitialApproximation()
{
	relaxationParam = 1.0;
	iterationsCounter = 0U;

	for (int i = 0; i < gridLinesNum; i++)
	{
		for (int j = 0; j < gridColumnsNum; j++)
		{
			lastValidValues[i][j] = 0.0;
		}
	}
}


double MagneticField::calcCoefficientIntegral(const Vector2& vert1, const Vector2& vert2, const Vector2& vert3, double doubleTriangleArea)
{
	return doubleTriangleArea * (1.0 + params.chi) * (vert1.r + vert2.r + vert3.r) / 6.0;
}


double MagneticField::calcCoefficient(int i, int j, int coefIndex)
{
	double result = 0.0;
	double doubleTriangleArea = 0.0;
	double integralVal = 0.0;

	Indices2 curIndices;
	Indices2 nextIndices;
	Vector2 vert1 = grid[i][j];
	Vector2 vert2;
	Vector2 vert3;

	if (coefIndex == 0)
	{
		double difR = 0.0;
		double difZ = 0.0;

		for (int k = 1; k < 7; k++)
		{
			curIndices = TABLE_OFFSETS[k];
			nextIndices = TABLE_OFFSETS[k + 1];

			curIndices.i += i;
			curIndices.j += j;

			nextIndices.i += i;
			nextIndices.j += j;

			if (!isIndicesValid(curIndices) || !isIndicesValid(nextIndices))
			{
				continue;
			}

			vert2 = grid[curIndices.i][curIndices.j];
			vert3 = grid[nextIndices.i][nextIndices.j];

			doubleTriangleArea = calcDoubleTriangleArea(vert1, vert2, vert3);
			integralVal = calcCoefficientIntegral(vert1, vert2, vert3, doubleTriangleArea);

			difZ = vert3.z - vert2.z;
			difR = vert3.r - vert2.r;

			result += (difZ * difZ + difR * difR) * integralVal / (doubleTriangleArea * doubleTriangleArea);
		}
	}
	else
	{
		curIndices = TABLE_OFFSETS[coefIndex - 1];
		nextIndices = TABLE_OFFSETS[coefIndex];

		curIndices.i += i;
		curIndices.j += j;

		nextIndices.i += i;
		nextIndices.j += j;

		if (isIndicesValid(curIndices) && isIndicesValid(nextIndices))
		{
			vert2 = grid[curIndices.i][curIndices.j];
			vert3 = grid[nextIndices.i][nextIndices.j];

			doubleTriangleArea = calcDoubleTriangleArea(vert1, vert2, vert3);
			integralVal = calcCoefficientIntegral(vert1, vert2, vert3, doubleTriangleArea);

			result += ((vert3.z - vert2.z) * (vert2.z - vert1.z) + (vert3.r - vert2.r) * (vert2.r - vert1.r)) *
				integralVal / (doubleTriangleArea * doubleTriangleArea);
		}

		curIndices = TABLE_OFFSETS[coefIndex];
		nextIndices = TABLE_OFFSETS[coefIndex + 1];

		curIndices.i += i;
		curIndices.j += j;

		nextIndices.i += i;
		nextIndices.j += j;

		if (isIndicesValid(curIndices) && isIndicesValid(nextIndices))
		{
			vert2 = grid[curIndices.i][curIndices.j];
			vert3 = grid[nextIndices.i][nextIndices.j];

			doubleTriangleArea = calcDoubleTriangleArea(vert1, vert2, vert3);
			integralVal = calcCoefficientIntegral(vert1, vert2, vert3, doubleTriangleArea);

			result += ((vert3.z - vert2.z) * (vert1.z - vert3.z) + (vert3.r - vert2.r) * (vert1.r - vert3.r)) *
				integralVal / (doubleTriangleArea * doubleTriangleArea);
		}
	}

	return result;
}


double MagneticField::calcNextValue(int i, int j)
{
	double result = 0.0;
	Indices2 resultIndices;

	for (int k = 2; k < 5; k++)
	{
		resultIndices = TABLE_OFFSETS[k];

		resultIndices.i += i;
		resultIndices.j += j;

		if (isIndicesValid(resultIndices))
		{
			result += calcCoefficient(i, j, k) * curApprox[resultIndices.i][resultIndices.j];
		}
	}

	for (int k = 5; k < 7; k++)
	{
		resultIndices = TABLE_OFFSETS[k];

		resultIndices.i += i;
		resultIndices.j += j;

		if (isIndicesValid(resultIndices))
		{
			result += calcCoefficient(i, j, k) * nextApprox[resultIndices.i][resultIndices.j];
		}
	}

	resultIndices = TABLE_OFFSETS[1];

	resultIndices.i += i;
	resultIndices.j += j;

	if (isIndicesValid(resultIndices))
	{
		result += calcCoefficient(i, j, 1) * nextApprox[resultIndices.i][resultIndices.j];
	}

	return -result / calcCoefficient(i, j, 0);
}


void MagneticField::calcNextApproximation()
{
	for (int i = 1; i < gridLinesNum; i++)
	{
		for (int j = gridColumnsNum - 2; j >= 0; j--)
		{
			nextApprox[i][j] = calcNextValue(i, j);
		}
	}
}


MagneticFieldResultCode MagneticField::calcRelaxation()
{
	double** tmpPtr;
	double curEpsilon = params.accuracy * relaxationParam;
	unsigned int counter = 0U;
	int limitColumns = gridColumnsNum - 1;

	for (int i = 0; i < gridLinesNum; i++)
	{
		for (int j = 0; j < gridColumnsNum; j++)
		{
			nextApprox[i][j] = lastValidValues[i][j];
			curApprox[i][j] = nextApprox[i][j];
		}
	}

	for (int i = 0; i < gridLinesNum; i++)
	{
		nextApprox[i][limitColumns] = grid[i][limitColumns].z;
		curApprox[i][limitColumns] = nextApprox[i][limitColumns];
	}

	for (int i = 0; i < gridColumnsNum; i++)
	{
		nextApprox[0][i] = 0.0;
		curApprox[0][i] = 0.0;
	}

	double b = 3.0 / (3.0 + params.chi);

	for (int i = 0; i < gridLinesNum; i++)
	{
//		nextApprox[i][surfaceColumnIndex] = b * grid[i][surfaceColumnIndex].z;
//		curApprox[i][surfaceColumnIndex] = b * grid[i][surfaceColumnIndex].z;

//		nextApprox[i][0] = b * grid[i][0].z;
//		curApprox[i][0] = b * grid[i][0].z;

		nextApprox[i][gridColumnsNum - 1] = grid[i][gridColumnsNum - 1].z - ((1.0 - b) * grid[i][gridColumnsNum - 1].z) / 
			pow(grid[i][gridColumnsNum - 1].z * grid[i][gridColumnsNum - 1].z + grid[i][gridColumnsNum - 1].r * grid[i][gridColumnsNum - 1].r, 3.0 / 2.0);
		curApprox[i][gridColumnsNum - 1] = nextApprox[i][gridColumnsNum - 1];
//		nextApprox[i][gridColumnsNum - 1] = grid[i][gridColumnsNum - 1].z;
//		curApprox[i][gridColumnsNum - 1] = grid[i][gridColumnsNum - 1].z;
	}

/*	for (int i = 0; i <= surfaceColumnIndex; i++)
	{
		nextApprox[gridLinesNum - 1][i] = b * grid[gridLinesNum - 1][i].z;
		curApprox[gridLinesNum - 1][i] = b * grid[gridLinesNum - 1][i].z;
	}
*/

/*	for (int i = 0; i < gridLinesNum; i++)
	{
		for (int j = 0; j < gridColumnsNum; j++)
		{
			nextApprox[i][j] = b * grid[i][j].z;
			curApprox[i][j] = b * grid[i][j].z;
		}
	}*/

	printf("================================= REAL ===================================\n");
	for (int i = gridLinesNum - 1; i >= 0; i--)
	{
		for (int j = 0; j < gridColumnsNum; j++)
		{
			printf("%lf ", b * grid[i][j].z);
		}

		printf("\n");
	}
	printf("\n");


	do
	{
		tmpPtr = nextApprox;
		nextApprox = curApprox;
		curApprox = tmpPtr;

		calcNextApproximation();
		relaxation(nextApprox, curApprox, gridLinesNum, gridColumnsNum, relaxationParam);

		printf("================================= RESULT ===================================\n");
		for (int i = gridLinesNum - 1; i >= 0; i--)
		{
			for (int j = 0; j < gridColumnsNum; j++)
			{
				printf("%lf ", nextApprox[i][j]);
			}

			printf("\n");
		}
		printf("\n");

		printf("================================= DELTA ===================================\n");
		for (int i = gridLinesNum - 1; i >= 0; i--)
		{
			for (int j = 0; j < gridColumnsNum; j++)
			{
				printf("%lf ", nextApprox[i][j] - (b * grid[i][j].z));
			}

			printf("\n");
		}
		printf("\n\n");

		printf("================================= SURFACE ===================================\n");
		for (int i = 0; i < gridLinesNum; i++)
		{
			printf("%lf %lf\n", grid[i][surfaceColumnIndex].r, grid[i][surfaceColumnIndex].z);
		}
		printf("\n\n");

		counter++;
	} while (norm(nextApprox, curApprox, gridLinesNum, surfaceColumnIndex + 1) > curEpsilon && counter < params.iterationsNumMax);

	iterationsCounter += counter;

	if (counter >= params.iterationsNumMax)
	{
		return FIELD_ITERATIONS_LIMIT_EXCEEDED;
	}
	else if (!isApproximationValid(nextApprox))
	{
		return FIELD_INVALID_RESULT;
	}
	else
	{
		tmpPtr = lastValidValues;
		lastValidValues = nextApprox;
		nextApprox = tmpPtr;

		for (int i = 0; i < gridLinesNum; i++)
		{
			for (int j = 0; j < gridColumnsNum; j++)
			{
				lastValidGrid[i][j] = grid[i][j];
			}
		}

		calcDerivatives();
	}

	return FIELD_SUCCESS;
}


bool MagneticField::isApproximationValid(double** approx)
{
	for (int i = 0; i < gridLinesNum; i++)
	{
		for (int j = 0; j < gridColumnsNum; j++)
		{
			if (!std::isfinite(approx[i][j]))
			{
				return false;
			}
		}
	}

	return true;
}


bool MagneticField::isIndicesValid(const Indices2& indices)
{
	return indices.i >= 0 && indices.j >= 0 && indices.i < gridLinesNum && indices.j < gridColumnsNum;
}


void MagneticField::calcDerivatives()
{
	int surfacePointsNum = params.surfaceSplitsNum + 1;
	int limit = surfacePointsNum - 1;
	double doubleArea = 0.0;

	Vector2 vert1;
	Vector2 vert2;
	Vector2 vert3;
	Vector2 vertMagneticR1;
	Vector2 vertMagneticR2;
	Vector2 vertMagneticR3;
	Vector2 vertMagneticZ1;
	Vector2 vertMagneticZ2;
	Vector2 vertMagneticZ3;
	Indices2 indicesOffset;

	outerDerivatives[0].r = (lastValidValues[0][surfaceColumnIndex + 1] - lastValidValues[0][surfaceColumnIndex]) /
							(lastValidGrid[0][surfaceColumnIndex + 1].r - lastValidGrid[0][surfaceColumnIndex].r);
	outerDerivatives[0].z = 0.0;

	for (int i = 0; i < limit; i++)
	{
		vert1 = lastValidGrid[i][surfaceColumnIndex];
		vertMagneticR1 = { lastValidValues[i][surfaceColumnIndex], vert1.z };
		vertMagneticZ1 = { vert1.r, lastValidValues[i][surfaceColumnIndex] };
		indicesOffset = TABLE_OFFSETS[3];
		vert2 = lastValidGrid[i + indicesOffset.i][surfaceColumnIndex + indicesOffset.j];
		vertMagneticR2 = { lastValidValues[i + indicesOffset.i][surfaceColumnIndex + indicesOffset.j], vert2.z };
		vertMagneticZ2 = { vert2.r, lastValidValues[i + indicesOffset.i][surfaceColumnIndex + indicesOffset.j] };
		indicesOffset = TABLE_OFFSETS[4];
		vert3 = lastValidGrid[i + indicesOffset.i][surfaceColumnIndex + indicesOffset.j];
		vertMagneticR3 = { lastValidValues[i + indicesOffset.i][surfaceColumnIndex + indicesOffset.j], vert3.z };
		vertMagneticZ3 = { vert3.r, lastValidValues[i + indicesOffset.i][surfaceColumnIndex + indicesOffset.j] };

		doubleArea = calcDoubleTriangleArea(vert1, vert2, vert3);

		innerDerivatives[i].r = calcDoubleTriangleArea(vertMagneticR1, vertMagneticR2, vertMagneticR3) / doubleArea;
		innerDerivatives[i].z = calcDoubleTriangleArea(vertMagneticZ1, vertMagneticZ2, vertMagneticZ3) / doubleArea;
	}


	for (int i = 1; i < surfacePointsNum; i++)
	{
		vert1 = lastValidGrid[i][surfaceColumnIndex];
		vertMagneticR1 = { lastValidValues[i][surfaceColumnIndex], vert1.z };
		vertMagneticZ1 = { vert1.r, lastValidValues[i][surfaceColumnIndex] };
		indicesOffset = TABLE_OFFSETS[6];
		vert2 = lastValidGrid[i + indicesOffset.i][surfaceColumnIndex + indicesOffset.j];
		vertMagneticR2 = { lastValidValues[i + indicesOffset.i][surfaceColumnIndex + indicesOffset.j], vert2.z };
		vertMagneticZ2 = { vert2.r, lastValidValues[i + indicesOffset.i][surfaceColumnIndex + indicesOffset.j] };
		indicesOffset = TABLE_OFFSETS[1];
		vert3 = lastValidGrid[i + indicesOffset.i][surfaceColumnIndex + indicesOffset.j];
		vertMagneticR3 = { lastValidValues[i + indicesOffset.i][surfaceColumnIndex + indicesOffset.j], vert3.z };
		vertMagneticZ3 = { vert3.r, lastValidValues[i + indicesOffset.i][surfaceColumnIndex + indicesOffset.j] };

		doubleArea = calcDoubleTriangleArea(vert1, vert2, vert3);

		outerDerivatives[i].r = calcDoubleTriangleArea(vertMagneticR1, vertMagneticR2, vertMagneticR3) / doubleArea;
		outerDerivatives[i].z = calcDoubleTriangleArea(vertMagneticZ1, vertMagneticZ2, vertMagneticZ3) / doubleArea;
	}

	innerDerivatives[limit].r = 0.0;
	innerDerivatives[limit].z = (lastValidValues[limit][surfaceColumnIndex + 1] - lastValidValues[limit][surfaceColumnIndex]) /
								(lastValidGrid[limit][surfaceColumnIndex + 1].z - lastValidGrid[limit][surfaceColumnIndex].z);
}