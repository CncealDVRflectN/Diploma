#ifndef DIPLOMA_PLOT_FIELD_ISOLINES_H
#define DIPLOMA_PLOT_FIELD_ISOLINES_H


#include <filesystem>
#include "Pipe.h"
#include "Plot.h"


class PlotFieldIsolines
{
public:
    PlotFieldIsolines(const PlotParams& params);


    void plot(const std::filesystem::path& fieldData, const std::filesystem::path& fluidData);

    void close();

private:
    Pipe mPipe;
    PlotParams mParams;

    
    std::string generateInternalContours(double startVal, double endVal) const;

    std::string generateExternalContours(double startVal, double endVal) const;
};

#endif