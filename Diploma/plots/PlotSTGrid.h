#ifndef DIPLOMA_PLOTSTGRID_H
#define DIPLOMA_PLOTSTGRID_H


#include <filesystem>
#include "Pipe.h"
#include "Plot.h"


class PlotSTGrid
{
public:
    PlotSTGrid(const PlotParams& params);
    
    
    void plot(const std::filesystem::path& internalDataPath, const std::filesystem::path& externalDataPath);
    
    void close();

private:
    Pipe mPipe;
    PlotParams mParams;
};

#endif