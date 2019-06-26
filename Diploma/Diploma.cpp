#include <iostream>
#include "util/util.h"
#include "problem/Solution.h"
#include "plot/PlotLines.h"
#include "plot/PlotGrid.h"
#include "plot/PlotIsolines.h"


enum OptsIds
{
	WINDOW_WIDTH_OPT = 1,
	WINDOW_HEIGHT_OPT,
	W_PARAM_INITIAL_OPT,
	W_PARAM_TARGET_OPT,
	CHI_INITIAL_OPT,
	CHI_TARGET_OPT,
	RELAXATION_PARAM_MIN_OPT,
	ACCURACY_OPT,
	SPLITS_NUM_OPT,
	ITERATIONS_MAX_NUM_OPT,
	W_RESULTS_NUM_OPT,
	CHI_RESULTS_NUM_OPT,
	FIELD_SURFACE_SPLITS_NUM_OPT,
	FIELD_INTERNAL_SPLITS_NUM_OPT,
	FIELD_INFINITY_SPLITS_NUM_OPT,
	FIELD_ITERATIONS_MAX_NUM_OPT,
	FIELD_ACCURACY_OPT,
	FIELD_RELAXATION_PARAM_MIN_OPT,
	EQUAL_AXIS_OPT,
	NONDIM_OPT
};


static const LongOpt LONG_OPTS[] = {
	{"window-width",			WINDOW_WIDTH_OPT},
	{"window-height",			WINDOW_HEIGHT_OPT},
	{"w-param-initial",			W_PARAM_INITIAL_OPT},
	{"w-param-target",			W_PARAM_TARGET_OPT},
	{"chi-param-initial",		CHI_INITIAL_OPT},
	{"chi-param-target",		CHI_TARGET_OPT},
	{"relaxation-param-min",	RELAXATION_PARAM_MIN_OPT},
	{"accuracy",				ACCURACY_OPT},
	{"splits-num",				SPLITS_NUM_OPT},
	{"iterations-max-num",		ITERATIONS_MAX_NUM_OPT},
	{"w-results-num",			W_RESULTS_NUM_OPT},
	{"chi-results-num",			CHI_RESULTS_NUM_OPT},
	{"field-surf-splits-num",	FIELD_SURFACE_SPLITS_NUM_OPT},
	{"field-int-splits-num",	FIELD_INTERNAL_SPLITS_NUM_OPT},
	{"field-inf-splits-num",	FIELD_INFINITY_SPLITS_NUM_OPT},
	{"field-iter-max-num",		FIELD_ITERATIONS_MAX_NUM_OPT},
	{"field-accuracy",			FIELD_ACCURACY_OPT},
	{"field-relax-param-min",	FIELD_RELAXATION_PARAM_MIN_OPT},
	{"equal-axis",				EQUAL_AXIS_OPT},
	{"nondim",					NONDIM_OPT},
	{nullptr,					0}
};


typedef struct program_params_t
{
	double wInitial;
	double wTarget;
	double relaxationParamMin;
	double fieldRelaxParamMin;
	double accuracy;
	double fieldAccuracy;
	double chiInitial;
	double chiTarget;
	int windowWidth;
	int windowHeight;
	int splitsNum;
	int fieldSurfaceSplitsNum;
	int fieldInternalSplitsNum;
	int fieldInfinitySplitsNum;
	int iterationsMaxNum;
	int fieldIterationsMaxNum;
	int resultsNumW;
	int resultsNumChi;
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
		case W_PARAM_INITIAL_OPT:
			programParams.wInitial = std::atof(optPtr);
			break;
		case W_PARAM_TARGET_OPT:
			programParams.wTarget = std::atof(optPtr);
			break;
		case CHI_INITIAL_OPT:
			programParams.chiInitial = std::atof(optPtr);
			break;
		case CHI_TARGET_OPT:
			programParams.chiTarget = std::atof(optPtr);
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
		case W_RESULTS_NUM_OPT:
			programParams.resultsNumW = std::atoi(optPtr);
			break;
		case CHI_RESULTS_NUM_OPT:
			programParams.resultsNumChi = std::atoi(optPtr);
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

	for (int i = 0; i < pointsNum; i++)
	{
		plotLine.points.push_back({ volumeNondimMul * result[i].r, volumeNondimMul * result[i].z });
	}

	sprintf(buffer, "W = %.2lf", fluid.getCurrentW());

	plotLine.title = buffer;

	plot.addLine(plotLine);
}


void addHeightCoefsLinePlot(const std::vector<Vector2>& heightCoefs, PlotLines& plot, double chi)
{
	char buffer[256];
	PlotLine plotLine;

	plotLine.points.assign(heightCoefs.begin(), heightCoefs.end());

	sprintf(buffer, "chi = %.2lf", chi);

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

	if (programParams.resultsNumW == 0 || programParams.resultsNumChi == 0)
	{
		printf("resultsNum = 0\n");
		return -1;
	}

	double curChi = 0.0;
	double stepChi = 0.0;

	if (programParams.resultsNumChi == 1)
	{
		stepChi = programParams.chiTarget - programParams.chiInitial;
		curChi = programParams.chiTarget;
	}
	else
	{
		stepChi = (programParams.chiTarget - programParams.chiInitial) / (programParams.resultsNumChi - 1);
		curChi = programParams.chiInitial;
	}

	ProblemParams problemParams;
	problemParams.accuracy = programParams.accuracy;
	problemParams.chi = curChi;
	problemParams.fieldAccuracy = programParams.fieldAccuracy;
	problemParams.fieldInfinitySplitsNum = programParams.fieldInfinitySplitsNum;
	problemParams.fieldInternalSplitsNum = programParams.fieldInternalSplitsNum;
	problemParams.fieldIterationsMaxNum = programParams.fieldIterationsMaxNum;
	problemParams.fieldRelaxParamMin = programParams.fieldRelaxParamMin;
	problemParams.fieldSurfaceSplitsNum = programParams.fieldSurfaceSplitsNum;
	problemParams.iterationsMaxNum = programParams.iterationsMaxNum;
	problemParams.relaxationParamMin = programParams.relaxationParamMin;
	problemParams.splitsNum = programParams.splitsNum;
	problemParams.wInitial = programParams.wInitial;
	problemParams.wTarget = programParams.wTarget;
	problemParams.resultsNum = programParams.resultsNumW;

	Solution solution(problemParams);

	PlotLinesParams heightCoefsPlotParams;
	heightCoefsPlotParams.windowWidth = programParams.windowWidth;
	heightCoefsPlotParams.windowHeight = programParams.windowHeight;
	heightCoefsPlotParams.title = "Коэффициент вытягивания";
	heightCoefsPlotParams.labelX = "W";
	heightCoefsPlotParams.labelY = "k";
	heightCoefsPlotParams.isEqualAxis = false;

	PlotLines heightCoefsPlot(heightCoefsPlotParams);

	for (int i = 0; i < programParams.resultsNumChi; i++)
	{
		ProblemResultCode resultCode = SUCCESS;

		solution.setChi(curChi);

		sprintf(buffer, "Осесимметричная задача (chi = %.1lf)", curChi);

		PlotLinesParams resultsPlotParams;
		resultsPlotParams.windowWidth = programParams.windowWidth;
		resultsPlotParams.windowHeight = programParams.windowHeight;
		resultsPlotParams.title = buffer;
		resultsPlotParams.labelX = "r";
		resultsPlotParams.labelY = "z";
		resultsPlotParams.isEqualAxis = programParams.isEqualAxis;

		PlotLines resultsPlot(resultsPlotParams);

		std::vector<Vector2> heightCoefs;

		solution.calcInitials();
		//solution.calcResult(0.0);
		addResultLinePlot(*(solution.getMagneticFluid()), resultsPlot, programParams.isNonDim);
		heightCoefs.push_back({ solution.getCurrentW(), calcHeightCoef(*(solution.getMagneticFluid())) });

		/*while (resultCode == SUCCESS)
		{
			resultCode = solution.calcNextResult();

			if (resultCode == SUCCESS || resultCode == TARGET_REACHED)
			{
				addResultLinePlot(*solution.getMagneticFluid(), resultsPlot, programParams.isNonDim);

				heightCoefs.push_back({ solution.getCurrentW(), calcHeightCoef(*solution.getMagneticFluid()) });
			}
		}

		addHeightCoefsLinePlot(heightCoefs, heightCoefsPlot, curChi);

		if (resultCode != TARGET_REACHED)
		{
			printf("Target W parameter can't be reached\n");

			system("pause");

			return -1;
		}*/

		PlotGridParams plotGridParams;
		plotGridParams.windowWidth = programParams.windowWidth;
		plotGridParams.windowHeight = programParams.windowHeight;
		plotGridParams.title = "Вычислительная сетка";
		plotGridParams.labelX = "r";
		plotGridParams.labelY = "z";
		plotGridParams.isEqualAxis = true;
		plotGridParams.volumeNonDimMul = (programParams.isNonDim) ? (*solution.getMagneticFluid()).calcVolumeNondimMul() : 1.0;

		PlotGrid plotGrid(plotGridParams);
		plotGrid.setGrid(solution.getLastValidFieldGrid(), solution.getGridLinesNum(),
			solution.getGridColumnsNum(), solution.getGridSurfaceColumnIndex());

		PlotIsolinesParams plotIsolinesParams;
		plotIsolinesParams.windowWidth = programParams.windowWidth;
		plotIsolinesParams.windowHeight = programParams.windowHeight;
		plotIsolinesParams.title = "Изолинии потенциала магнитного поля";
		plotIsolinesParams.labelX = "r";
		plotIsolinesParams.labelY = "z";
		plotIsolinesParams.isEqualAxis = true;
		plotIsolinesParams.volumeNonDimMul = (programParams.isNonDim) ? (*solution.getMagneticFluid()).calcVolumeNondimMul() : 1.0;

		PlotIsolines plotIsolines(plotIsolinesParams);
		plotIsolines.setGridAndValues(solution.getLastValidFieldGrid(), solution.getLastValidFieldPotential(),
			solution.getGridLinesNum(), solution.getGridColumnsNum(),
			solution.getGridSurfaceColumnIndex());

		resultsPlot.plot();
		plotGrid.plot();
		plotIsolines.plot();

		system("pause");

		resultsPlot.close();
		plotGrid.close();
		plotIsolines.close();

		curChi += stepChi;
	}

	heightCoefsPlot.plot();

	system("pause");

	heightCoefsPlot.close();

	return 0;
}
