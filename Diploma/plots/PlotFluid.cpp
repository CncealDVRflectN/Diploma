#include "PlotFluid.h"
#include "files_util.h"


#pragma region Constructors

PlotFluid::PlotFluid(const PlotParams& params) : mParams(params), mPipe("gnuplot", PipeAccessType::PIPE_WRITE)
{}

#pragma endregion


#pragma region Member methods

void PlotFluid::plot(const std::vector<std::filesystem::path>& fluidDatas)
{
    if (fluidDatas.empty())
    {
        std::cout << "WARNING: Nothing to plot" << std::endl;
        return;
    }

    std::vector<FluidResultParams> params;

    for (const std::filesystem::path& path : fluidDatas)
    {
        params.push_back(read_fluid_params(path));
    }

    mPipe.write("set term wxt size %u,%u enhanced font 'Verdana,10'\n", mParams.windowWidth, mParams.windowHeight);
    mPipe.write("set datafile commentschars '%c'\n", COMMENT_CHARACTER);
    mPipe.write("load '%s'\n", plot_config_path("fluid.cfg").string().c_str());
    mPipe.write("set title 'Fluid surface dynamics, {/Symbol c}=%lf'\n", params[0].chi);
    mPipe.write("set xlabel '%s' norotate textcolor rgb '#757575'\n", params[0].xLabel.c_str());
    mPipe.write("set ylabel '%s' norotate textcolor rgb '#757575'\n", params[0].yLabel.c_str());

    if (mParams.isEqualAxis)
    {
        mPipe.write("set size ratio -1\n");
    }

    size_t linesNum = params.size();

    mPipe.write("plot ");
    for (size_t i = 0; i < linesNum; i++)
    {
        if (i > 0)
        {
            mPipe.write(", ");
        }

        mPipe.write("'%s' u 1:2:(%lf) with lines title 'W=%lf' lw 2 palette", fluidDatas[i].string().c_str(), 
                                                                              (double)(i + 1) / linesNum, params[i].w);
    }
    mPipe.write("\n");

    mPipe.flush();
}


void PlotFluid::close()
{
    mPipe.close();
}

#pragma endregion
