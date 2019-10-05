#include "ProgramOptsHandler.h"
#include "program_opts.h"


enum OptsIds
{
    WINDOW_WIDTH_OPT = 1,
    WINDOW_HEIGHT_OPT,
    W_PARAM_TARGET_OPT,
    CHI_INITIAL_OPT,
    CHI_TARGET_OPT,
    RELAXATION_PARAM_INITIAL_OPT,
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
    FIELD_RELAXATION_PARAM_INITIAL_OPT,
    FIELD_RELAXATION_PARAM_MIN_OPT,
    FIELD_INFINITY_POS_MULTIPLIER,
    EQUAL_AXIS_OPT,
    NONDIM_OPT,
    PEDANTIC_RIGHT_SWEEP_OPT
};


static const LongOpt LONG_OPTS[] = {
    {"window-width",			    WINDOW_WIDTH_OPT},
    {"window-height",			    WINDOW_HEIGHT_OPT},
    {"w-param-target",			    W_PARAM_TARGET_OPT},
    {"chi-param-initial",		    CHI_INITIAL_OPT},
    {"chi-param-target",		    CHI_TARGET_OPT},
    {"relaxation-param-initial",    RELAXATION_PARAM_INITIAL_OPT},
    {"relaxation-param-min",	    RELAXATION_PARAM_MIN_OPT},
    {"accuracy",				    ACCURACY_OPT},
    {"splits-num",				    SPLITS_NUM_OPT},
    {"iterations-max-num",		    ITERATIONS_MAX_NUM_OPT},
    {"w-results-num",			    W_RESULTS_NUM_OPT},
    {"chi-results-num",			    CHI_RESULTS_NUM_OPT},
    {"field-surf-splits-num",	    FIELD_SURFACE_SPLITS_NUM_OPT},
    {"field-int-splits-num",	    FIELD_INTERNAL_SPLITS_NUM_OPT},
    {"field-inf-splits-num",	    FIELD_INFINITY_SPLITS_NUM_OPT},
    {"field-iter-max-num",		    FIELD_ITERATIONS_MAX_NUM_OPT},
    {"field-accuracy",			    FIELD_ACCURACY_OPT},
    {"field-relax-param-initial",   FIELD_RELAXATION_PARAM_INITIAL_OPT},
    {"field-relax-param-min",	    FIELD_RELAXATION_PARAM_MIN_OPT},
    {"field-inf-pos-multiplier",    FIELD_INFINITY_POS_MULTIPLIER},
    {"equal-axis",				    EQUAL_AXIS_OPT},
    {"nondim",					    NONDIM_OPT},
    {"pedantic-right-sweep",        PEDANTIC_RIGHT_SWEEP_OPT},
    {nullptr,					    0}
};



#pragma region Constructors

ProgramOptsHandler::ProgramOptsHandler()
{
    mParams.windowWidth = 1280;
    mParams.windowHeight = 720;
    mParams.wTarget = 0.0;
    mParams.relaxationParamInitial = 1.0;
    mParams.relaxationParamMin = 0.1;
    mParams.fieldRelaxParamInitial = 1.0;
    mParams.fieldRelaxParamMin = 0.1;
    mParams.accuracy = 0.1;
    mParams.fieldAccuracy = 0.1;
    mParams.chiInitial = 0.0;
    mParams.chiTarget = 0.0;
    mParams.splitsNum = 100;
    mParams.fieldSurfaceSplitsNum = 10;
    mParams.fieldInternalSplitsNum = 5;
    mParams.fieldInfinitySplitsNum = 5;
    mParams.iterationsMaxNum = 1000;
    mParams.fieldIterationsMaxNum = 1000;
    mParams.fieldInfinityPosMultiplier = 4.0;
    mParams.resultsNumW = 1;
    mParams.resultsNumChi = 1;
    mParams.isEqualAxis = false;
    mParams.isNonDim = false;
    mParams.isRightSweepPedantic = false;
}


ProgramOptsHandler::ProgramOptsHandler(const ProgramParams& params)
{
    mParams = params;
}

#pragma endregion


#pragma region Getters

ProgramParams ProgramOptsHandler::parameters() const
{
    return mParams;
}


ProblemParams ProgramOptsHandler::problemParameters() const
{
    ProblemParams problemParams;

    problemParams.accuracy = mParams.accuracy;
    problemParams.fieldAccuracy = mParams.fieldAccuracy;
    problemParams.chi = mParams.chiInitial;
    problemParams.wTarget = mParams.wTarget;
    problemParams.relaxationParamInitial = mParams.relaxationParamInitial;
    problemParams.fieldRelaxParamInitial = mParams.fieldRelaxParamInitial;
    problemParams.relaxationParamMin = mParams.relaxationParamMin;
    problemParams.fieldRelaxParamMin = mParams.fieldRelaxParamMin;
    problemParams.iterationsMaxNum = mParams.iterationsMaxNum;
    problemParams.fieldIterationsMaxNum = mParams.fieldIterationsMaxNum;
    problemParams.resultsNum = mParams.resultsNumW;
    problemParams.splitsNum = mParams.splitsNum;
    problemParams.gridParams.surfaceSplitsNum = mParams.fieldSurfaceSplitsNum;
    problemParams.gridParams.internalSplitsNum = mParams.fieldInternalSplitsNum;
    problemParams.gridParams.infSplitsNum = mParams.fieldInfinitySplitsNum;
    problemParams.gridParams.infMultiplier = mParams.fieldInfinityPosMultiplier;
    problemParams.isRightSweepPedantic = mParams.isRightSweepPedantic;

    return problemParams;
}


PlotLinesParams ProgramOptsHandler::heightCoefsPlotParameters() const
{
    PlotLinesParams plotParams;

    plotParams.windowWidth = mParams.windowWidth;
    plotParams.windowHeight = mParams.windowHeight;
    plotParams.title = "Коэффициент вытягивания";
    plotParams.labelX = "W";
    plotParams.labelY = "k";
    plotParams.isEqualAxis = false;

    return plotParams;
}


PlotLinesParams ProgramOptsHandler::surfacePlotParametes(std::string title) const
{
    PlotLinesParams plotParams;

    plotParams.windowWidth = mParams.windowWidth;
    plotParams.windowHeight = mParams.windowHeight;
    plotParams.title = title;
    plotParams.labelX = "r";
    plotParams.labelY = "z";
    plotParams.isEqualAxis = mParams.isEqualAxis;

    return plotParams;
}


PlotSTGridParams ProgramOptsHandler::gridPlotParameters() const
{
    PlotSTGridParams plotParams;

    plotParams.windowWidth = mParams.windowWidth;
    plotParams.windowHeight = mParams.windowHeight;
    plotParams.title = "Вычислительная сетка";
    plotParams.labelX = "r";
    plotParams.labelY = "z";
    plotParams.isEqualAxis = mParams.isEqualAxis;

    return plotParams;
}


PlotIsolinesParams ProgramOptsHandler::isolinesPlotParameters() const
{
    PlotIsolinesParams plotParams;

    plotParams.windowWidth = mParams.windowWidth;
    plotParams.windowHeight = mParams.windowHeight;
    plotParams.title = "Изолинии потенциала магнитного поля";
    plotParams.labelX = "r";
    plotParams.labelY = "z";
    plotParams.isEqualAxis = mParams.isEqualAxis;

    return plotParams;
}

#pragma endregion


#pragma region Parse options

void ProgramOptsHandler::parseOpts(int argsNum, char** args)
{
    int optId = 0;
    char* optPtr = nullptr;

    for (int i = 0; i < argsNum; i++)
    {
        optPtr = getOpt(args[i], nullptr, LONG_OPTS, optId);

        if (optPtr != nullptr)
        {
            handleOpt(optId, optPtr);
        }
        else
        {
            printf("Unknown parameter: %s \n", args[i]);
        }
    }

    if (mParams.wTarget == 0.0)
    {
        mParams.resultsNumW = 1;
    }

    if (mParams.chiTarget - mParams.chiInitial == 0.0)
    {
        mParams.resultsNumChi = 1;
    }
}


void ProgramOptsHandler::handleOpt(int optId, char* optPtr)
{
    switch (optId)
    {
    case WINDOW_WIDTH_OPT:
        mParams.windowWidth = std::atoi(optPtr);
        break;
    case WINDOW_HEIGHT_OPT:
        mParams.windowHeight = std::atoi(optPtr);
        break;
    case W_PARAM_TARGET_OPT:
        mParams.wTarget = std::atof(optPtr);
        break;
    case CHI_INITIAL_OPT:
        mParams.chiInitial = std::atof(optPtr);
        break;
    case CHI_TARGET_OPT:
        mParams.chiTarget = std::atof(optPtr);
        break;
    case RELAXATION_PARAM_INITIAL_OPT:
        mParams.relaxationParamInitial = std::atof(optPtr);
        break;
    case RELAXATION_PARAM_MIN_OPT:
        mParams.relaxationParamMin = std::atof(optPtr);
        break;
    case ACCURACY_OPT:
        mParams.accuracy = std::atof(optPtr);
        break;
    case SPLITS_NUM_OPT:
        mParams.splitsNum = std::atoi(optPtr);
        break;
    case ITERATIONS_MAX_NUM_OPT:
        mParams.iterationsMaxNum = std::atoi(optPtr);
        break;
    case W_RESULTS_NUM_OPT:
        mParams.resultsNumW = std::atoi(optPtr);
        break;
    case CHI_RESULTS_NUM_OPT:
        mParams.resultsNumChi = std::atoi(optPtr);
        break;
    case FIELD_SURFACE_SPLITS_NUM_OPT:
        mParams.fieldSurfaceSplitsNum = std::atoi(optPtr);
        break;
    case FIELD_INTERNAL_SPLITS_NUM_OPT:
        mParams.fieldInternalSplitsNum = std::atoi(optPtr);
        break;
    case FIELD_INFINITY_SPLITS_NUM_OPT:
        mParams.fieldInfinitySplitsNum = std::atoi(optPtr);
        break;
    case FIELD_ACCURACY_OPT:
        mParams.fieldAccuracy = std::atof(optPtr);
        break;
    case FIELD_ITERATIONS_MAX_NUM_OPT:
        mParams.fieldIterationsMaxNum = std::atoi(optPtr);
        break;
    case FIELD_RELAXATION_PARAM_INITIAL_OPT:
        mParams.fieldRelaxParamInitial = std::atof(optPtr);
        break;
    case FIELD_RELAXATION_PARAM_MIN_OPT:
        mParams.fieldRelaxParamMin = std::atof(optPtr);
        break;
    case FIELD_INFINITY_POS_MULTIPLIER:
        mParams.fieldInfinityPosMultiplier = std::atof(optPtr);
        break;
    case EQUAL_AXIS_OPT:
        mParams.isEqualAxis = true;
        break;
    case NONDIM_OPT:
        mParams.isNonDim = true;
        break;
    case PEDANTIC_RIGHT_SWEEP_OPT:
        mParams.isRightSweepPedantic = true;
        break;
    default:
        break;
    }
}

#pragma endregion
