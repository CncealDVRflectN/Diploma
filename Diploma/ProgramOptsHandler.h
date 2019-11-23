#ifndef DIPLOMA_PROGRAM_OPTS_HANDLER_H
#define DIPLOMA_PROGRAM_OPTS_HANDLER_H

#include "Solution.h"
#include "Plot.h"


typedef struct program_params_t
{
    std::string xLabel;
    std::string yLabel;
    std::string potentialLabel;
    std::string errorLabel;
    double wTarget;
    double relaxationParamInitial;
    double relaxationParamMin;
    double fieldRelaxParamInitial;
    double fieldRelaxParamMin;
    double fieldModelRelaxParamInitial;
    double fieldModelRelaxParamMin;
    double accuracy;
    double fieldAccuracy;
    double chiInitial;
    double chiTarget;
    double fieldModelChi;
    double fieldInfinityPosMultiplier;
    int windowWidth;
    int windowHeight;
    int splitsNum;
    int fieldSurfaceSplitsNum;
    int fieldInternalSplitsNum;
    int fieldExternalSplitsNum;
    int iterationsMaxNum;
    int fieldIterationsMaxNum;
    int resultsNumW;
    int resultsNumChi;
    bool isEqualAxis;
    bool isDimensionless;
    bool isRightSweepPedantic;
    bool isMainProblemEnabled;
    bool isFieldModelProblemEnabled;
    bool isPlotFluidSurfaceEnabled;
    bool isPlotFieldGridEnabled;
    bool isPlotFieldEnabled;
    bool isPlotFieldIsolinesEnabled;
    bool isPlotFieldErrorEnabled;
    bool isPlotHeightCoefsEnabled;
} ProgramParams;


class ProgramOptsHandler
{
public:
    ProgramOptsHandler();

    ProgramOptsHandler(const ProgramParams& params);


    ProgramParams parameters() const;

    ProblemParams problemParameters() const;

    PlotParams plotParameters() const;


    void parseOpts(int argsNum, char** args);

private:
    ProgramParams mParams;


    std::string readStringValue(char* optPtr) const noexcept(false);

    void handleOpt(int optId, char* optPtr);
};

#endif