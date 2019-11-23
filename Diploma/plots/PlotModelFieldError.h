#ifndef DIPLOMA_PLOT_MODEL_FIELD_ERROR_H
#define DIPLOMA_PLOT_MODEL_FIELD_ERROR_H


#include <filesystem>
#include "Plot.h"
#include "Pipe.h"


class PlotModelFieldError
{
public:
    PlotModelFieldError(const PlotParams& params);


    void plot(const std::filesystem::path& errorData, const std::filesystem::path& fluidData);

    void close();

private:
    Pipe mPipe;
    PlotParams mParams;
};

#endif