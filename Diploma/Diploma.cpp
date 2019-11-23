#include <iostream>
#include "ProgramOptsHandler.h"
#include "PlotSTGrid.h"
#include "PlotFluid.h"
#include "PlotHeightCoefs.h"
#include "PlotFieldIsolines.h"
#include "PlotField.h"
#include "PlotModelFieldError.h"


void calculateFieldModelProblem(const ProgramOptsHandler& optsHandler, Solution& solution)
{
    ProgramParams programParams = optsHandler.parameters();

    ResultCode resultCode = solution.calcFieldModelProblem();

    if (resultCode != ResultCode::FIELD_SUCCESS)
    {
        printf("WARNING: Could not calculate field model problem\n\n");
        return;
    }

    std::filesystem::path fieldPath = intermediate_file_path(generate_file_name("field-model", "dat", "-"));
    std::filesystem::path errorPath = intermediate_file_path(generate_file_name("field-model", "dat", "-", "error"));
    std::filesystem::path gridIntPath = intermediate_file_path(generate_file_name("field-model", "dat", "-", "grid", "int"));
    std::filesystem::path gridExtPath = intermediate_file_path(generate_file_name("field-model", "dat", "-", "grid", "ext"));

    printf("Saving results to files...\n");

    solution.writeFieldData(fieldPath);
    solution.writeFieldErrorData(errorPath);
    solution.writeInternalGridData(gridIntPath);
    solution.writeExternalGridData(gridExtPath);

    std::filesystem::path fluidPath = solution.writeFluidData();

    printf("Results saved\n\n");

    PlotParams plotParams = optsHandler.plotParameters();
    PlotSTGrid plotGrid(plotParams);
    PlotFieldIsolines plotFieldIsolines(plotParams);
    PlotField plotField(plotParams);
    PlotModelFieldError plotError(plotParams);

    if (programParams.isPlotFieldGridEnabled)
    {
        printf("Plotting magnetic field grid...\n\n");
        plotGrid.plot(gridIntPath, gridExtPath);
    }

    if (programParams.isPlotFieldIsolinesEnabled)
    {
        printf("Plotting magnetic field isolines...\n\n");
        plotFieldIsolines.plot(fieldPath, fluidPath);
    }

    if (programParams.isPlotFieldEnabled)
    {
        printf("Plotting magnetic field...\n\n");
        plotField.plot(fieldPath, fluidPath);
    }

    if (programParams.isPlotFieldErrorEnabled)
    {
        printf("Plotting magnetic field error...\n\n");
        plotError.plot(errorPath, fluidPath);
    }

    system("pause");

    plotGrid.close();
    plotFieldIsolines.close();
    plotField.close();
    plotError.close();
}


void calculateMainProblem(const ProgramOptsHandler& optsHandler, Solution& solution)
{
    ProgramParams programParams = optsHandler.parameters();

    if (programParams.resultsNumW == 0 || programParams.resultsNumChi == 0)
    {
        printf("Number of results is zero\n\n");
        return;
    }

    PlotParams plotsParams = optsHandler.plotParameters();
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

    std::vector<std::filesystem::path> heightCoefsDatas;

    for (int i = 0; i < programParams.resultsNumChi; i++)
    {
        std::vector<Vector2<double>> heightCoefs;
        std::vector<std::filesystem::path> fluidDatas;
        ResultCode resultCode = ResultCode::SUCCESS;
        double volumeNonDimMul = 1.0;

        solution.resetIterationsCounters();
        solution.setChi(curChi);
        solution.calcInitials();
        resultCode = solution.calcResult(0.0);

        if ((resultCode == ResultCode::SUCCESS && programParams.resultsNumW > 1) || resultCode == ResultCode::TARGET_REACHED)
        {
            fluidDatas.push_back(solution.writeFluidData());
            heightCoefs.push_back({ solution.currentW(), solution.heightCoef() });
        }

        while (resultCode == ResultCode::SUCCESS)
        {
            resultCode = solution.calcNextResult();

            if (resultCode == ResultCode::SUCCESS || resultCode == ResultCode::TARGET_REACHED)
            {
                fluidDatas.push_back(solution.writeFluidData());
                heightCoefs.push_back({ solution.currentW(), solution.heightCoef() });
            }
        }

        if (resultCode != ResultCode::TARGET_REACHED)
        {
            printf("Target W parameter can't be reached\n\n");
            system("pause");
            return;
        }

        printf("Saving results to files...\n");

        std::filesystem::path fieldData = solution.writeFieldData();
        std::filesystem::path internalGridData = solution.writeInternalGridData();
        std::filesystem::path externalGridData = solution.writeExternalGridData();
        
        heightCoefsDatas.push_back(write_height_coefs_data(curChi, heightCoefs));

        printf("Results saved\n\n");

        PlotFluid fluidPlot(plotsParams);
        PlotSTGrid gridPlot(plotsParams);
        PlotFieldIsolines isolinesPlot(plotsParams);
        PlotField fieldPlot(plotsParams);

        if (programParams.isPlotFluidSurfaceEnabled)
        {
            printf("Plotting fluid surface...\n\n");
            fluidPlot.plot(fluidDatas);
        }

        if (programParams.isPlotFieldGridEnabled)
        {
            printf("Plotting magnetic field grid...\n\n");
            gridPlot.plot(internalGridData, externalGridData);
        }

        if (programParams.isPlotFieldIsolinesEnabled)
        {
            printf("Plotting magnetic field isolines...\n\n");
            isolinesPlot.plot(fieldData, fluidDatas.back());
        }

        if (programParams.isPlotFieldEnabled)
        {
            printf("Plotting magnetic field...\n\n");
            fieldPlot.plot(fieldData, fluidDatas.back());
        }

        system("pause");

        fluidPlot.close();
        gridPlot.close();
        isolinesPlot.close();
        fieldPlot.close();

        curChi += stepChi;
    }

    PlotHeightCoefs heightCoefsPlot(plotsParams);

    if (programParams.isPlotHeightCoefsEnabled)
    {
        printf("Plotting fluid height to width ratios...\n\n");
        heightCoefsPlot.plot(heightCoefsDatas);
    }

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
