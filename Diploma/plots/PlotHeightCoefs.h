#ifndef DIPLOMA_PLOT_HEIGHT_COEFS_H
#define DIPLOMA_PLOT_HEIGHT_COEFS_H


#include <filesystem>
#include "Pipe.h"
#include "Plot.h"


class PlotHeightCoefs
{
public:
    PlotHeightCoefs(const PlotParams& params);


    void plot(const std::vector<std::filesystem::path>& heightCoefsDatas);

    void close();

private:
    Pipe mPipe;
    PlotParams mParams;
};

#endif