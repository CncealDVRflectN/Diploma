#include "PlotSTGrid.h"
#include "files_util.h"


#pragma region Constructors

PlotSTGrid::PlotSTGrid(const PlotParams& params) : mParams(params), mPipe("gnuplot", PipeAccessType::PIPE_WRITE)
{}

#pragma endregion


#pragma region Member methods

void PlotSTGrid::plot(const std::filesystem::path& internalDataPath, const std::filesystem::path& externalDataPath)
{
    FieldResultParams params = read_field_grid_params(internalDataPath);

    mPipe.write("set term wxt size %u, %u enhanced font 'Verdana,10'\n", mParams.windowWidth, mParams.windowHeight);
    mPipe.write("set datafile commentschars '%c'\n", COMMENT_CHARACTER);
    mPipe.write("load '%s'\n", plot_config_path("st-grid.cfg").string().c_str());
    mPipe.write("set title 'Magnetic field grid, {/Symbol c}=%lf'\n", params.chi);
    mPipe.write("set xlabel '%s' norotate textcolor rgb '#757575'\n", params.xLabel.c_str());
    mPipe.write("set ylabel '%s' norotate textcolor rgb '#757575'\n", params.yLabel.c_str());

    if (mParams.isEqualAxis)
    {
        mPipe.write("set size ratio -1\n");
    }

    mPipe.write("plot '%s' u 1:2 with lines title 'Internal grid' ls @INTERNAL_STYLE, "
                "'%s' u 1:2 with lines title 'External grid' ls @EXTERNAL_STYLE\n", 
                internalDataPath.string().c_str(), externalDataPath.string().c_str());

    mPipe.flush();
}


void PlotSTGrid::close()
{
    mPipe.close();
}

#pragma endregion
