#ifndef DIPLOMA_PROGRAM_OPTS_HANDLER_H
#define DIPLOMA_PROGRAM_OPTS_HANDLER_H

#include "Solution.h"
#include "PlotLines.h"
#include "PlotSTGrid.h";
#include "PlotIsolines.h"


typedef struct program_params_t
{
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
    int fieldInfinitySplitsNum;
    int iterationsMaxNum;
    int fieldIterationsMaxNum;
    int resultsNumW;
    int resultsNumChi;
    bool isEqualAxis;
    bool isNonDim;
    bool isRightSweepPedantic;
    bool isMainProblemEnabled;
    bool isFieldModelProblemEnabled;
} ProgramParams;


class ProgramOptsHandler
{
public:
    ProgramOptsHandler();

    ProgramOptsHandler(const ProgramParams& params);


    ProgramParams parameters() const;

    ProblemParams problemParameters() const;

    PlotLinesParams heightCoefsPlotParameters() const;

    PlotLinesParams surfacePlotParametes(std::string title) const;

    PlotSTGridParams gridPlotParameters() const;

    PlotIsolinesParams isolinesPlotParameters() const;


    void parseOpts(int argsNum, char** args);

private:
    ProgramParams mParams;


    void handleOpt(int optId, char* optPtr);
};

#endif