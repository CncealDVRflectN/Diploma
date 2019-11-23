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
    FIELD_EXTERNAL_SPLITS_NUM_OPT,
    FIELD_ITERATIONS_MAX_NUM_OPT,
    FIELD_ACCURACY_OPT,
    FIELD_RELAXATION_PARAM_INITIAL_OPT,
    FIELD_RELAXATION_PARAM_MIN_OPT,
    FIELD_MODEL_RELAXATION_PARAM_INITIAL_OPT,
    FIELD_MODEL_RELAXATION_PARAM_MIN_OPT,
    FIELD_MODEL_CHI_OPT,
    FIELD_INFINITY_POS_MULTIPLIER_OPT,
    EQUAL_AXIS_OPT,
    DIMENSIONLESS_OPT,
    PEDANTIC_RIGHT_SWEEP_OPT,
    MAIN_PROBLEM_OPT,
    FIELD_MODEL_PROBLEM_OPT,
    LABEL_X_OPT,
    LABEL_Y_OPT,
    LABEL_POTENTIAL_OPT,
    LABEL_ERROR_OPT
};


static const LongOpt LONG_OPTS[] = {
    {"window-width",			            WINDOW_WIDTH_OPT},
    {"window-height",			            WINDOW_HEIGHT_OPT},
    {"w-param-target",			            W_PARAM_TARGET_OPT},
    {"chi-param-initial",		            CHI_INITIAL_OPT},
    {"chi-param-target",		            CHI_TARGET_OPT},
    {"relaxation-param-initial",            RELAXATION_PARAM_INITIAL_OPT},
    {"relaxation-param-min",	            RELAXATION_PARAM_MIN_OPT},
    {"accuracy",				            ACCURACY_OPT},
    {"splits-num",				            SPLITS_NUM_OPT},
    {"iterations-max-num",		            ITERATIONS_MAX_NUM_OPT},
    {"w-results-num",			            W_RESULTS_NUM_OPT},
    {"chi-results-num",			            CHI_RESULTS_NUM_OPT},
    {"field-surf-splits-num",	            FIELD_SURFACE_SPLITS_NUM_OPT},
    {"field-int-splits-num",	            FIELD_INTERNAL_SPLITS_NUM_OPT},
    {"field-ext-splits-num",	            FIELD_EXTERNAL_SPLITS_NUM_OPT},
    {"field-iter-max-num",		            FIELD_ITERATIONS_MAX_NUM_OPT},
    {"field-accuracy",			            FIELD_ACCURACY_OPT},
    {"field-relax-param-initial",           FIELD_RELAXATION_PARAM_INITIAL_OPT},
    {"field-relax-param-min",	            FIELD_RELAXATION_PARAM_MIN_OPT},
    {"field-model-relax-param-initial",     FIELD_MODEL_RELAXATION_PARAM_INITIAL_OPT},
    {"field-model-relax-param-min",         FIELD_MODEL_RELAXATION_PARAM_MIN_OPT},
    {"field-model-chi",                     FIELD_MODEL_CHI_OPT},
    {"field-inf-pos-multiplier",            FIELD_INFINITY_POS_MULTIPLIER_OPT},
    {"equal-axis",				            EQUAL_AXIS_OPT},
    {"dimensionless",					    DIMENSIONLESS_OPT},
    {"pedantic-right-sweep",                PEDANTIC_RIGHT_SWEEP_OPT},
    {"main-problem",                        MAIN_PROBLEM_OPT},
    {"field-model-problem",                 FIELD_MODEL_PROBLEM_OPT},
    {"label-x",                             LABEL_X_OPT},
    {"label-y",                             LABEL_Y_OPT},
    {"label-potential",                     LABEL_POTENTIAL_OPT},
    {"label-error",                         LABEL_ERROR_OPT},
    {nullptr,					            0}
};



#pragma region Constructors

ProgramOptsHandler::ProgramOptsHandler()
{
    mParams.xLabel = "x";
    mParams.yLabel = "y";
    mParams.potentialLabel = "potential";
    mParams.errorLabel = "error";
    mParams.windowWidth = 1280;
    mParams.windowHeight = 720;
    mParams.wTarget = 0.0;
    mParams.relaxationParamInitial = 1.0;
    mParams.relaxationParamMin = 0.1;
    mParams.fieldRelaxParamInitial = 1.0;
    mParams.fieldRelaxParamMin = 0.1;
    mParams.fieldModelRelaxParamInitial = 1.0;
    mParams.fieldModelRelaxParamMin = 0.1;
    mParams.accuracy = 0.1;
    mParams.fieldAccuracy = 0.1;
    mParams.chiInitial = 1.0;
    mParams.chiTarget = 1.0;
    mParams.splitsNum = 100;
    mParams.fieldSurfaceSplitsNum = 10;
    mParams.fieldInternalSplitsNum = 5;
    mParams.fieldExternalSplitsNum = 5;
    mParams.iterationsMaxNum = 1000;
    mParams.fieldIterationsMaxNum = 1000;
    mParams.fieldModelChi = 1.0;
    mParams.fieldInfinityPosMultiplier = 4.0;
    mParams.resultsNumW = 1;
    mParams.resultsNumChi = 1;
    mParams.isEqualAxis = false;
    mParams.isDimensionless = false;
    mParams.isRightSweepPedantic = false;
    mParams.isMainProblemEnabled = false;
    mParams.isFieldModelProblemEnabled = false;
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

    problemParams.xLabel = mParams.xLabel;
    problemParams.yLabel = mParams.yLabel;
    problemParams.potentialLabel = mParams.potentialLabel;
    problemParams.errorLabel = mParams.errorLabel;
    problemParams.accuracy = mParams.accuracy;
    problemParams.fieldAccuracy = mParams.fieldAccuracy;
    problemParams.chi = mParams.chiInitial;
    problemParams.wTarget = mParams.wTarget;
    problemParams.relaxationParamInitial = mParams.relaxationParamInitial;
    problemParams.fieldRelaxParamInitial = mParams.fieldRelaxParamInitial;
    problemParams.fieldModelRelaxParamInitial = mParams.fieldModelRelaxParamInitial;
    problemParams.relaxationParamMin = mParams.relaxationParamMin;
    problemParams.fieldRelaxParamMin = mParams.fieldRelaxParamMin;
    problemParams.fieldModelRelaxParamMin = mParams.fieldModelRelaxParamMin;
    problemParams.fieldModelChi = mParams.fieldModelChi;
    problemParams.iterationsMaxNum = mParams.iterationsMaxNum;
    problemParams.fieldIterationsMaxNum = mParams.fieldIterationsMaxNum;
    problemParams.resultsNum = mParams.resultsNumW;
    problemParams.splitsNum = mParams.splitsNum;
    problemParams.gridParams.surfaceSplitsNum = mParams.fieldSurfaceSplitsNum;
    problemParams.gridParams.internalSplitsNum = mParams.fieldInternalSplitsNum;
    problemParams.gridParams.externalSplitsNum = mParams.fieldExternalSplitsNum;
    problemParams.gridParams.infMultiplier = mParams.fieldInfinityPosMultiplier;
    problemParams.isRightSweepPedantic = mParams.isRightSweepPedantic;
    problemParams.isDimensionless = mParams.isDimensionless;

    return problemParams;
}


PlotParams ProgramOptsHandler::plotParameters() const
{
    PlotParams plotParams;

    plotParams.windowWidth = mParams.windowWidth;
    plotParams.windowHeight = mParams.windowHeight;
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

        case FIELD_EXTERNAL_SPLITS_NUM_OPT:
            mParams.fieldExternalSplitsNum = std::atoi(optPtr);
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

        case FIELD_MODEL_RELAXATION_PARAM_INITIAL_OPT:
            mParams.fieldModelRelaxParamInitial = std::atof(optPtr);
            break;

        case FIELD_MODEL_RELAXATION_PARAM_MIN_OPT:
            mParams.fieldModelRelaxParamMin = std::atof(optPtr);
            break;

        case FIELD_MODEL_CHI_OPT:
            mParams.fieldModelChi = std::atof(optPtr);
            break;

        case FIELD_INFINITY_POS_MULTIPLIER_OPT:
            mParams.fieldInfinityPosMultiplier = std::atof(optPtr);
            break;

        case LABEL_X_OPT:
            mParams.xLabel = readStringValue(optPtr);
            break;

        case LABEL_Y_OPT:
            mParams.yLabel = readStringValue(optPtr);
            break;

        case LABEL_POTENTIAL_OPT:
            mParams.potentialLabel = readStringValue(optPtr);
            break;

        case LABEL_ERROR_OPT:
            mParams.errorLabel = readStringValue(optPtr);
            break;
        
        case EQUAL_AXIS_OPT:
            mParams.isEqualAxis = true;
            break;

        case DIMENSIONLESS_OPT:
            mParams.isDimensionless = true;
            break;

        case PEDANTIC_RIGHT_SWEEP_OPT:
            mParams.isRightSweepPedantic = true;
            break;

        case MAIN_PROBLEM_OPT:
            mParams.isMainProblemEnabled = true;
            break;

        case FIELD_MODEL_PROBLEM_OPT:
            mParams.isFieldModelProblemEnabled = true;
            break;

        default:
            break;
    }
}

#pragma endregion


#pragma region Values reading

std::string ProgramOptsHandler::readStringValue(char* optPtr) const noexcept(false)
{
    std::string result;

    if (*optPtr != '\'')
    {
        throw std::runtime_error("Unrecognized string option semantic");
    }

    optPtr++;

    while (*optPtr != '\'' && *optPtr != '\0')
    {
        if (*optPtr == '\\' && *(optPtr + 1) == '\'')
        {
            optPtr++;
        }

        result += *optPtr;
        optPtr++;
    }

    return result;
}

#pragma endregion
