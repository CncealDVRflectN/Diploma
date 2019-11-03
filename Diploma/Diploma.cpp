#include <iostream>
#include "files_util.h"
#include "ProgramOptsHandler.h"


void calculateFieldModelProblem(const ProgramOptsHandler& optsHandler, Solution& solution)
{
    ProgramParams programParams = optsHandler.parameters();

    ResultCode resultCode = solution.calcFieldModelProblem();

    if (resultCode != FIELD_SUCCESS)
    {
        printf("Could not calculate field model problem\n");
        return;
    }

    PlotSTGridParams plotGridParams = optsHandler.gridPlotParameters();
    PlotSTGrid plotGrid(plotGridParams);

    PlotIsolinesParams plotIsolinesParams = optsHandler.isolinesPlotParameters();
    PlotIsolines plotIsolines(plotIsolinesParams);

    double volumeNonDimMul = (programParams.isNonDim) ? solution.volumeNonDimMul() : 1.0;

    printf("Plotting grid...\n");
    plotGrid.plot(solution.field().grid(), volumeNonDimMul);
    printf("Plotting isolines...\n\n");
    plotIsolines.plot(solution.field().grid(), solution.field().lastValidResult(), volumeNonDimMul);

    system("pause");

    plotGrid.close();
    plotIsolines.close();
}


void calculateMainProblem(const ProgramOptsHandler& optsHandler, Solution& solution)
{
    ProgramParams programParams = optsHandler.parameters();

    if (programParams.resultsNumW == 0 || programParams.resultsNumChi == 0)
    {
        printf("Number of results is zero\n\n");
        return;
    }

    char buffer[256];
    double curChi = programParams.chiInitial;
    double stepChi = 0.0;

    if (programParams.resultsNumChi == 1)
    {
        stepChi = 0.0;
        curChi = programParams.chiTarget;
    }
    else
    {
        stepChi = (programParams.chiTarget - programParams.chiInitial) / (programParams.resultsNumChi - 1);
        curChi = programParams.chiInitial;
    }

    PlotLinesParams heightCoefsPlotParams = optsHandler.heightCoefsPlotParameters();
    PlotLines heightCoefsPlot(heightCoefsPlotParams);

    std::vector<PlotLine> heightCoefsLines;

    for (int i = 0; i < programParams.resultsNumChi; i++)
    {
        std::vector<Vector2<double>> heightCoefs;
        std::vector<PlotLine> surfaceLines;
        ResultCode resultCode = SUCCESS;
        double volumeNonDimMul = 1.0;

        sprintf(buffer, "Осесимметричная задача (chi = %.1lf)", curChi);

        PlotLinesParams resultsPlotParams = optsHandler.surfacePlotParametes(buffer);
        PlotLines resultsPlot(resultsPlotParams);

        solution.resetIterationsCounters();
        solution.setChi(curChi);
        solution.calcInitials();
        resultCode = solution.calcResult(0.0);

        if ((resultCode == SUCCESS && programParams.resultsNumW > 1) || resultCode == TARGET_REACHED)
        {
            sprintf(buffer, "W = %.2lf", solution.currentW());

            volumeNonDimMul = (programParams.isNonDim) ? solution.volumeNonDimMul() : 1.0;
            surfaceLines.push_back({ buffer, volumeNonDimMul * solution.fluidSurface() });
            heightCoefs.push_back({ solution.currentW(), solution.heightCoef() });
        }

        while (resultCode == SUCCESS)
        {
            resultCode = solution.calcNextResult();

            if (resultCode == SUCCESS || resultCode == TARGET_REACHED)
            {
                sprintf(buffer, "W = %.2lf", solution.currentW());

                volumeNonDimMul = (programParams.isNonDim) ? solution.volumeNonDimMul() : 1.0;
                surfaceLines.push_back({ buffer, volumeNonDimMul * solution.fluidSurface() });
                heightCoefs.push_back({ solution.currentW(), solution.heightCoef() });
            }
        }

        sprintf(buffer, "chi = %.2lf", curChi);
        heightCoefsLines.push_back({ buffer, heightCoefs });

        if (resultCode != TARGET_REACHED)
        {
            printf("Target W parameter can't be reached\n\n");
            system("pause");
            return;
        }

        PlotSTGridParams plotGridParams = optsHandler.gridPlotParameters();
        PlotSTGrid plotGrid(plotGridParams);

        PlotIsolinesParams plotIsolinesParams = optsHandler.isolinesPlotParameters();
        PlotIsolines plotIsolines(plotIsolinesParams);

        printf("Plotting surface lines...\n");
        resultsPlot.plot(surfaceLines);
        printf("Plotting grid...\n");
        plotGrid.plot(solution.field().grid(), volumeNonDimMul);
        printf("Plotting isolines...\n\n");
        plotIsolines.plot(solution.field().grid(), solution.field().lastValidResult(), volumeNonDimMul);

        system("pause");

        resultsPlot.close();
        plotGrid.close();
        plotIsolines.close();

        curChi += stepChi;
    }

    printf("Plotting height coefficient lines...\n\n");
    heightCoefsPlot.plot(heightCoefsLines);

    system("pause");

    heightCoefsPlot.close();
}


int main(int argc, char** argv)
{
    ProgramOptsHandler optsHandler;

    printf("Clearing intermediate files...\n");
    clear_intermediate_files();
    printf("Intermediate files cleared\n\n");

    printf("Parsing options...\n");
    optsHandler.parseOpts(argc, argv);
    printf("Options parsed\n\n");

    ProgramParams programParams = optsHandler.parameters();
	Solution solution(optsHandler.problemParameters());

    if (programParams.isFieldModelProblemEnabled)
    {
        printf("Calculating field model problem...\n");
        calculateFieldModelProblem(optsHandler, solution);
        printf("Field model problem calculations completed\n\n");
    }

    if (programParams.isMainProblemEnabled)
    {
        printf("Calculating main problem...\n");
        calculateMainProblem(optsHandler, solution);
        printf("Main problem calculations completed\n\n");
    }

	return 0;
}
