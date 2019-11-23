#include "PlotHeightCoefs.h"
#include "files_util.h"


#pragma region Constructors

PlotHeightCoefs::PlotHeightCoefs(const PlotParams& params) : mParams(params), mPipe("gnuplot", PipeAccessType::PIPE_WRITE)
{}

#pragma endregion


#pragma region Member methods

void PlotHeightCoefs::plot(const std::vector<std::filesystem::path>& heightCoefsDatas)
{
    if (heightCoefsDatas.empty())
    {
        std::cout << "WARNING: Nothing to plot" << std::endl;
        return;
    }

    std::vector<double> chis;

    for (const std::filesystem::path& path : heightCoefsDatas)
    {
        chis.push_back(read_height_coefs_params(path));
    }

    mPipe.write("set term wxt size %u,%u enhanced font 'Verdana,10'\n", mParams.windowWidth, mParams.windowHeight);
    mPipe.write("set datafile commentschars '%c'\n", COMMENT_CHARACTER);
    mPipe.write("load '%s'\n", plot_config_path("height-coefs.cfg").string().c_str());
    mPipe.write("set title 'Fluid height to width ratio'\n");
    mPipe.write("set xlabel 'W' norotate textcolor rgb '#757575'\n");
    mPipe.write("set ylabel 'k' norotate textcolor rgb '#757575'\n");

    size_t linesNum = chis.size();

    mPipe.write("plot ");
    for (size_t i = 0; i < linesNum; i++)
    {
        if (i > 0)
        {
            mPipe.write(", ");
        }

        mPipe.write("'%s' u 1:2:(%lf) with lines title '{/Symbol c}=%lf' lw 2 palette", heightCoefsDatas[i].string().c_str(), 
                                                                                        (double)(i + 1) / linesNum, chis[i]);
    }
    mPipe.write("\n");

    mPipe.flush();
}


void PlotHeightCoefs::close()
{
    mPipe.close();
}

#pragma endregion
