#include "PlotModelFieldError.h"
#include "files_util.h"


#pragma region Constructors

PlotModelFieldError::PlotModelFieldError(const PlotParams& params) : mParams(params), mPipe("gnuplot", PipeAccessType::PIPE_WRITE)
{}

#pragma endregion


#pragma region Member methods

void PlotModelFieldError::plot(const std::filesystem::path& errorData, const std::filesystem::path& fluidData)
{
    const static Vector2<int> gridSize(60, 60);

    FieldModelParams modelParams = read_field_model_params(errorData);
    FluidResultParams fluidParams = read_fluid_params(fluidData);
    std::filesystem::path errorGridData = intermediate_file_path("error-grid-data.dat");

    mPipe.write("set term wxt size %u,%u enhanced font 'Verdana,10'\n", mParams.windowWidth, mParams.windowHeight);
    mPipe.write("set datafile commentschars '%c'\n", COMMENT_CHARACTER);

    mPipe.write("set table '%s'\n", errorGridData.string().c_str());
    mPipe.write("set dgrid3d %d,%d splines\n", gridSize.x, gridSize.y);

    mPipe.write("splot '%s' using 1:2:3\n", errorData.string().c_str());

    mPipe.write("unset table\n");
    mPipe.write("unset dgrid3d\n");

    mPipe.write("set pm3d map interpolate 6,6\n");

    mPipe.write("load '%s'\n", plot_config_path("field-error.cfg").string().c_str());
    mPipe.write("set title 'Magnetic field error, {/Symbol c}=%lf'\n", modelParams.chi);
    mPipe.write("set xlabel '%s' norotate textcolor rgb '#757575'\n", modelParams.xLabel.c_str());
    mPipe.write("set ylabel '%s' norotate textcolor rgb '#757575'\n", modelParams.yLabel.c_str());
    mPipe.write("set zlabel '%s' norotate textcolor rgb '#757575'\n", modelParams.errorLabel.c_str());
    mPipe.write("set cbrange [%lf:%lf]\n", modelParams.errorMin, modelParams.errorMax);

    if (mParams.isEqualAxis)
    {
        mPipe.write("set size ratio -1\n");
    }

    mPipe.write("set multiplot\n");

    mPipe.write("splot '%s' using 1:2:3 with pm3d notitle, "
                "'%s' using 1:2:(0.0) with lines notitle ls @SURFACE_STYLE\n",
                errorGridData.string().c_str(),
                fluidData.string().c_str());

    mPipe.flush();
}


void PlotModelFieldError::close()
{
    mPipe.close();
}

#pragma endregion
