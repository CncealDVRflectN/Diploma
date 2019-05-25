#include <iostream>
#include "util/util.h"
#include "problem/MagneticFluid.h"
#include "plot/PlotLines.h"
#include "plot/PlotGrid.h"


enum OptsIds
{
	WINDOW_WIDTH_OPT = 1,
	WINDOW_HEIGHT_OPT,
	ANGLE_OPT,
	TARGET_BOND_OPT,
	BOND_STEP_OPT,
	RELAXATION_PARAM_MIN_OPT,
	ACCURACY_OPT,
	SPLITS_NUM_OPT,
	ITERATIONS_MAX_NUM_OPT,
	RESULTS_NUM_OPT,
	FIELD_SURFACE_SPLITS_NUM_OPT,
	FIELD_INTERNAL_SPLITS_NUM_OPT,
	FIELD_INFINITY_SPLITS_NUM_OPT,
	FIELD_ITERATIONS_MAX_NUM_OPT,
	FIELD_ACCURACY_OPT,
	FIELD_RELAXATION_PARAM_MIN_OPT,
	FIELD_CHI_OPT,
	FIELD_W_OPT,
	EQUAL_AXIS_OPT,
	NONDIM_OPT
};


static const LongOpt LONG_OPTS[] = {
	{"window-width",			WINDOW_WIDTH_OPT},
	{"window-height",			WINDOW_HEIGHT_OPT},
	{"angle",					ANGLE_OPT},
	{"target-bond",				TARGET_BOND_OPT},
	{"bond-step",				BOND_STEP_OPT},
	{"relaxation-param-min",	RELAXATION_PARAM_MIN_OPT},
	{"accuracy",				ACCURACY_OPT},
	{"splits-num",				SPLITS_NUM_OPT},
	{"iterations-max-num",		ITERATIONS_MAX_NUM_OPT},
	{"results-num",				RESULTS_NUM_OPT},
	{"field-surf-splits-num",	FIELD_SURFACE_SPLITS_NUM_OPT},
	{"field-int-splits-num",	FIELD_INTERNAL_SPLITS_NUM_OPT},
	{"field-inf-splits-num",	FIELD_INFINITY_SPLITS_NUM_OPT},
	{"field-iter-max-num",		FIELD_ITERATIONS_MAX_NUM_OPT},
	{"field-accuracy",			FIELD_ACCURACY_OPT},
	{"field-relax-param-min",	FIELD_RELAXATION_PARAM_MIN_OPT},
	{"field-chi",				FIELD_CHI_OPT},
	{"field-w",					FIELD_W_OPT},
	{"equal-axis",				EQUAL_AXIS_OPT},
	{"nondim",					NONDIM_OPT},
	{nullptr,					0}
};


typedef struct program_params_t
{
	double angle;
	double targetBond;
	double bondStep;
	double relaxationParamMin;
	double fieldRelaxParamMin;
	double accuracy;
	double fieldAccuracy;
	double chi;
	double w;
	int windowWidth;
	int windowHeight;
	int splitsNum;
	int fieldSurfaceSplitsNum;
	int fieldInternalSplitsNum;
	int fieldInfinitySplitsNum;
	int iterationsMaxNum;
	int fieldIterationsMaxNum;
	int resultsNum;
	bool isEqualAxis;
	bool isNonDim;
} ProgramsParams;


void handleOpts(int optId, char* optPtr, ProgramsParams& programParams)
{
	switch (optId)
	{
		case WINDOW_WIDTH_OPT:
			programParams.windowWidth = std::atoi(optPtr);
			break;
		case WINDOW_HEIGHT_OPT:
			programParams.windowHeight = std::atoi(optPtr);
			break;
		case ANGLE_OPT:
			programParams.angle = std::atof(optPtr) * M_PI / 180.0;
			break;
		case TARGET_BOND_OPT:
			programParams.targetBond = std::atof(optPtr);
			break;
		case BOND_STEP_OPT:
			programParams.bondStep = std::atof(optPtr);
			break;
		case RELAXATION_PARAM_MIN_OPT:
			programParams.relaxationParamMin = std::atof(optPtr);
			break;
		case ACCURACY_OPT:
			programParams.accuracy = std::atof(optPtr);
			break;
		case SPLITS_NUM_OPT:
			programParams.splitsNum = std::atoi(optPtr);
			break;
		case ITERATIONS_MAX_NUM_OPT:
			programParams.iterationsMaxNum = std::atoi(optPtr);
			break;
		case RESULTS_NUM_OPT:
			programParams.resultsNum = std::atoi(optPtr);
			break;
		case FIELD_SURFACE_SPLITS_NUM_OPT:
			programParams.fieldSurfaceSplitsNum = std::atoi(optPtr);
			break;
		case FIELD_INTERNAL_SPLITS_NUM_OPT:
			programParams.fieldInternalSplitsNum = std::atoi(optPtr);
			break;
		case FIELD_INFINITY_SPLITS_NUM_OPT:
			programParams.fieldInfinitySplitsNum = std::atoi(optPtr);
			break;
		case FIELD_ACCURACY_OPT:
			programParams.fieldAccuracy = std::atof(optPtr);
			break;
		case FIELD_ITERATIONS_MAX_NUM_OPT:
			programParams.fieldIterationsMaxNum = std::atoi(optPtr);
			break;
		case FIELD_RELAXATION_PARAM_MIN_OPT:
			programParams.fieldRelaxParamMin = std::atof(optPtr);
			break;
		case FIELD_CHI_OPT:
			programParams.chi = std::atof(optPtr);
			break;
		case FIELD_W_OPT:
			programParams.w = std::atof(optPtr);
			break;
		case EQUAL_AXIS_OPT:
			programParams.isEqualAxis = true;
			break;
		case NONDIM_OPT:
			programParams.isNonDim = true;
			break;
		default:
			break;
	}
}


void parseArgs(int argc, char** argv, ProgramsParams& programParams)
{
	int optId = 0;
	char* optPtr = nullptr;

	for (int i = 0; i < argc; i++)
	{
		optPtr = getOpt(argv[i], nullptr, LONG_OPTS, optId);

		if (optPtr != nullptr)
		{
			handleOpts(optId, optPtr, programParams);
		}
		else
		{
			printf("Unknown parameter: %s \n", argv[i]);
		}
	}
}


void addResultLinePlot(MagneticFluid& fluid, PlotLines& plot, bool isNonDim)
{
	int pointsNum = fluid.getPointsNum();
	char buffer[256];
	double volumeNondimMul = (isNonDim) ? fluid.calcVolumeNondimMul() : 1.0;
	const Vector2* result = fluid.getLastValidResult();
	PlotLine plotLine;
	std::vector<Vector2> resultVector(pointsNum);

	for (int i = 0; i < pointsNum; i++)
	{
		plotLine.points.push_back({ volumeNondimMul * result[i].r, volumeNondimMul * result[i].z });
	}

	sprintf(buffer, "Bo = %.2lf", fluid.getCurrentBond());

	plotLine.title = buffer;

	plot.addLine(plotLine);
}


double calcHeightCoef(MagneticFluid& fluid)
{
	int pointsNum = fluid.getPointsNum();
	const Vector2* result = fluid.getLastValidResult();

	return result[0].z / result[pointsNum - 1].r;
}


int main(int argc, char** argv)
{
	ProgramsParams programParams;
	char buffer[256];

	programParams.windowWidth = 1280;
	programParams.windowHeight = 720;
	programParams.isEqualAxis = false;
	programParams.isNonDim = false;

	parseArgs(argc, argv, programParams);

	FluidParams fluidParams;
	fluidParams.alpha = programParams.angle;
	fluidParams.chi = programParams.chi;
	fluidParams.w = programParams.w;
	fluidParams.bond = programParams.targetBond;
	fluidParams.bondStep = programParams.bondStep;
	fluidParams.epsilon = programParams.accuracy;
	fluidParams.iterationsNumMax = programParams.iterationsMaxNum;
	fluidParams.relaxParamMin = programParams.relaxationParamMin;
	fluidParams.splitsNum = programParams.splitsNum;

	MagneticParams magneticParams;
	magneticParams.accuracy = programParams.fieldAccuracy;
	magneticParams.chi = programParams.chi;
	magneticParams.surfaceSplitsNum = programParams.fieldSurfaceSplitsNum;
	magneticParams.internalSplitsNum = programParams.fieldInternalSplitsNum;
	magneticParams.infSplitsNum = programParams.fieldInfinitySplitsNum;
	magneticParams.iterationsNumMax = programParams.fieldIterationsMaxNum;
	magneticParams.relaxParamMin = programParams.fieldRelaxParamMin;

	MagneticFluid fluid(fluidParams, magneticParams);
	FluidResultCode resultCode = FLUID_SUCCESS;

	sprintf(buffer, "Осесимметричная задача (a = %.1lf)", 180.0 * programParams.angle / M_PI);

	PlotLinesParams resultsPlotParams;
	resultsPlotParams.windowWidth = programParams.windowWidth;
	resultsPlotParams.windowHeight = programParams.windowHeight;
	resultsPlotParams.title = buffer;
	resultsPlotParams.labelX = "r";
	resultsPlotParams.labelY = "z";
	resultsPlotParams.isEqualAxis = programParams.isEqualAxis;

	PlotLines resultsPlot(resultsPlotParams);

	PlotLinesParams heightCoefsPlotParams;
	heightCoefsPlotParams.windowWidth = programParams.windowWidth;
	heightCoefsPlotParams.windowHeight = programParams.windowHeight;
	heightCoefsPlotParams.title = "Коэффициент сжатия";
	heightCoefsPlotParams.labelX = "Bo";
	heightCoefsPlotParams.labelY = "k";
	heightCoefsPlotParams.isEqualAxis = false;

	PlotLines heightCoefsPlot(heightCoefsPlotParams);

	int possibleResultsNum = (int)(programParams.targetBond / programParams.bondStep) + 1;
	int resultsNum = (programParams.resultsNum <= possibleResultsNum) ? programParams.resultsNum : possibleResultsNum;
	int pointsNum = fluid.getPointsNum();
	double resultsBondStep = programParams.targetBond / (resultsNum - 1);
	double nextResultBond = 0.0;
	double curBond = 0.0;
	double volumeNondimMul = 0.0;
	std::vector<Vector2> heightCoefs;

	while (resultCode == FLUID_SUCCESS)
	{
		resultCode = fluid.calcNextValidResult();
		curBond = fluid.getCurrentBond();

		if (curBond >= nextResultBond && (resultCode == FLUID_SUCCESS || resultCode == FLUID_TARGET_BOND_REACHED))
		{
			addResultLinePlot(fluid, resultsPlot, programParams.isNonDim);

			nextResultBond += resultsBondStep;
		}

		heightCoefs.push_back({ curBond, calcHeightCoef(fluid) });
	}

	if (resultCode != FLUID_SUCCESS && resultCode != FLUID_TARGET_BOND_REACHED)
	{
		printf("Target bond number can't be reached\n");

		system("pause");

		return -1;
	}

	PlotGridParams plotGridParams;
	plotGridParams.windowWidth = programParams.windowWidth;
	plotGridParams.windowHeight = programParams.windowHeight;
	plotGridParams.title = "Вычислительная сетка";
	plotGridParams.labelX = "r";
	plotGridParams.labelY = "z";
	plotGridParams.isEqualAxis = true;

	MagneticField* magneticField = fluid.getMagneticField();

	PlotGrid plotGrid(plotGridParams);
	plotGrid.setGrid(magneticField->getLastValidGrid(), magneticField->getGridLinesNum(), magneticField->getGridColumnsNum());

	PlotLine heightCoefPlotLine;
	heightCoefPlotLine.points = heightCoefs;
	heightCoefPlotLine.title = "Осесимметричная задача";

	heightCoefsPlot.addLine(heightCoefPlotLine);

	resultsPlot.plot();
	heightCoefsPlot.plot();
	plotGrid.plot();

	system("pause");

	resultsPlot.close();
	heightCoefsPlot.close();
	plotGrid.close();

	return 0;
}
