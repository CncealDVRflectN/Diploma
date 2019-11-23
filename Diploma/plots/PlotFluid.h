#ifndef DIPLOMA_PLOT_FLUID_H
#define DIPLOMA_PLOT_FLUID_H


#include <filesystem>
#include "Pipe.h"
#include "Plot.h"


class PlotFluid
{
public:
    PlotFluid(const PlotParams& params);


    void plot(const std::vector<std::filesystem::path>& fluidDatas);

    void close();

private:
    Pipe mPipe;
    PlotParams mParams;
};

#endif