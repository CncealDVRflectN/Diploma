#include "Solution.h"


//Solution::Solution(const ProblemParams& params)
//{
//	this->params = params;
//
//	if (params.resultsNum == 1)
//	{
//		stepW = params.wTarget - params.wInitial;
//		curW = params.wTarget;
//	}
//	else
//	{
//		stepW = (params.wTarget - params.wInitial) / (params.resultsNum - 1);
//		curW = params.wInitial;
//	}
//
//	field = new MagneticField(getFieldParams(params));
//	fluid = new MagneticFluid(getFluidParams(params));
//
//	int gridLinesNum = field->getGridLinesNum();
//	int gridColumnsNum = field->getGridColumnsNum();
//
//	lastValidFluidSurface = new Vector2[params.splitsNum + 1];
//
//	lastValidFieldGrid = new Vector2*[gridLinesNum];
//	lastValidFieldPotential = new double*[gridLinesNum];
//
//	for (int i = 0; i < gridLinesNum; i++)
//	{
//		lastValidFieldGrid[i] = new Vector2[gridColumnsNum];
//		lastValidFieldPotential[i] = new double[gridColumnsNum];
//	}
//}
//
//
//Solution::~Solution()
//{
//	int gridLinesNum = field->getGridLinesNum();
//	int gridColumnsNum = field->getGridColumnsNum();
//
//	for (int i = 0; i < gridLinesNum; i++)
//	{
//		delete lastValidFieldGrid[i];
//		delete lastValidFieldPotential[i];
//	}
//
//	delete lastValidFieldGrid;
//	delete lastValidFieldPotential;
//	delete lastValidFluidSurface;
//
//	delete field;
//	delete fluid;
//}
//
//
//MagneticFluid* Solution::getMagneticFluid()
//{
//	return fluid;
//}
//
//
//MagneticField* Solution::getMagneticField()
//{
//	return field;
//}
//
//
//Vector2* Solution::getLastValidSurface()
//{
//	return lastValidFluidSurface;
//}
//
//
//Vector2** Solution::getLastValidFieldGrid()
//{
//	return lastValidFieldGrid;
//}
//
//
//double** Solution::getLastValidFieldPotential()
//{
//	return lastValidFieldPotential;
//}
//
//
//double Solution::getCurrentW()
//{
//	return curW;
//}
//
//
//int Solution::getSurfacePointsNum()
//{
//	return fluid->getPointsNum();
//}
//
//
//int Solution::getGridLinesNum()
//{
//	return field->getGridLinesNum();
//}
//
//
//int Solution::getGridColumnsNum()
//{
//	return field->getGridColumnsNum();
//}
//
//
//int Solution::getGridSurfaceColumnIndex()
//{
//	return field->getSurfaceColumnIndex();
//}
//
//
//void Solution::setChi(double chi)
//{
//	params.chi = chi;
//	fluid->setChi(chi);
//	field->setChi(chi);
//}
//
//
//void Solution::calcInitials()
//{
//	fluid->setRelaxationParam(1.0);
//	field->setRelaxationParam(1.0);
//
//	fluid->setW(curW);
//	fluid->calcInitialApproximation();
//	field->generateGrid(fluid->getLastValidResult(), fluid->getPointsNum());
//	field->calcInitialApproximation();
//	field->calcRelaxation();
//	calcDerivatives();
//
//	updateLastValidResults();
//}
//
//
//ProblemResultCode Solution::calcResult(double w)
//{
//	ProblemResultCode resultCode = INVALID_RESULT;
//
//	fluid->setW(w);
//
//	while (resultCode != SUCCESS &&
//		fluid->getCurrentRelaxationParam() >= params.relaxationParamMin &&
//		field->getCurrentRelaxationParam() >= params.fieldRelaxParamMin)
//	{
//		resultCode = fluid->calcRelaxation();
//
//		if (resultCode == FLUID_SUCCESS)
//		{
//			field->generateGrid(fluid->getLastValidResult(), fluid->getPointsNum());
//			resultCode = field->calcRelaxation();
//
//			if (resultCode == FIELD_SUCCESS)
//			{
//				if (isAccuracyReached())
//				{
//					resultCode = SUCCESS;
//				}
//				else
//				{
//					resultCode = ACCURACY_NOT_REACHED;
//				}
//
//				updateLastValidResults();
//				calcDerivatives();
//			}
//			else
//			{
//				fluid->setLastValidResult(lastValidFluidSurface);
//				field->setLastValidResult(lastValidFieldPotential);
//				field->setGrid(lastValidFieldGrid);
//
//				field->setRelaxationParam(0.5 * field->getCurrentRelaxationParam());
//			}
//		}
//		else
//		{
//			fluid->setLastValidResult(lastValidFluidSurface);
//			field->setLastValidResult(lastValidFieldPotential);
//			field->setGrid(lastValidFieldGrid);
//
//			fluid->setRelaxationParam(0.5 * fluid->getCurrentRelaxationParam());
//		}
//	}
//
//	if (resultCode == SUCCESS)
//	{
//		curW = w;
//
//		if (curW >= params.wTarget || fabs(curW - params.wTarget) <= 0.00001)
//		{
//			resultCode = TARGET_REACHED;
//		}
//	}
//	else
//	{
//		fluid->setW(curW);
//	}
//
//	return resultCode;
//}
//
//
//ProblemResultCode Solution::calcNextResult()
//{
//	double nextW = curW + stepW;
//	return calcResult(nextW);
//}
//
//
//FluidParams Solution::getFluidParams(const ProblemParams& problemParams)
//{
//	FluidParams fluidParams;
//
//	fluidParams.w = problemParams.wInitial;
//	fluidParams.chi = problemParams.chi;
//	fluidParams.epsilon = problemParams.accuracy;
//	fluidParams.iterationsNumMax = problemParams.iterationsMaxNum;
//	fluidParams.relaxParamMin = problemParams.relaxationParamMin;
//	fluidParams.splitsNum = problemParams.splitsNum;
//
//	return fluidParams;
//}
//
//
//MagneticParams Solution::getFieldParams(const ProblemParams& problemParams)
//{
//	MagneticParams fieldParams;
//
//	fieldParams.chi = problemParams.chi;
//	fieldParams.accuracy = problemParams.fieldAccuracy;
//	fieldParams.infSplitsNum = problemParams.fieldInfinitySplitsNum;
//	fieldParams.internalSplitsNum = problemParams.fieldInternalSplitsNum;
//	fieldParams.surfaceSplitsNum = problemParams.fieldSurfaceSplitsNum;
//	fieldParams.iterationsNumMax = problemParams.fieldIterationsMaxNum;
//	fieldParams.relaxParamMin = problemParams.fieldRelaxParamMin;
//
//	return fieldParams;
//}
//
//
//void Solution::updateLastValidResults()
//{
//	Vector2* surface = fluid->getLastValidResult();
//	Vector2** grid = field->getGrid();
//	double** potential = field->getLastValidResult();
//	int surfacePointsNum = fluid->getPointsNum();
//	int gridLinesNum = field->getGridLinesNum();
//	int gridColumnsNum = field->getGridColumnsNum();
//
//	for (int i = 0; i < surfacePointsNum; i++)
//	{
//		lastValidFluidSurface[i] = surface[i];
//	}
//
//	for (int i = 0; i < gridLinesNum; i++)
//	{
//		for (int j = 0; j < gridColumnsNum; j++)
//		{
//			lastValidFieldGrid[i][j] = grid[i][j];
//			lastValidFieldPotential[i][j] = potential[i][j];
//		}
//	}
//}
//
//
//void Solution::calcDerivatives()
//{
//	Vector2* derivatives = fluid->getDerivatives();
//	int pointsNum = fluid->getPointsNum();
//	double param = 0.0;
//
//	for (int i = 0; i < pointsNum; i++)
//	{
//		param = i / (pointsNum - 1);
//
//		derivatives[i] = field->calcInnerDerivative(param);
//	}
//}
//
//
//bool Solution::isAccuracyReached()
//{
//	return norm(fluid->getLastValidResult(), lastValidFluidSurface, fluid->getPointsNum()) <= 10.0 * params.accuracy &&
//		   norm(field->getLastValidResult(), lastValidFieldPotential, field->getGridLinesNum(), field->getGridColumnsNum()) <= 10.0 * params.fieldAccuracy;
//}
