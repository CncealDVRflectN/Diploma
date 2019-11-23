#ifndef DIPLOMA_PLOT_FIELD_H
#define DIPLOMA_PLOT_FIELD_H


#include <filesystem>
#include "Plot.h"
#include "Pipe.h"


class PlotField
{
public:
    PlotField(const PlotParams& params);

    
    void plot(const std::filesystem::path& fieldData, const std::filesystem::path& fluidData);

    void close();

private:
    Pipe mPipe;
    PlotParams mParams;
};

#endif