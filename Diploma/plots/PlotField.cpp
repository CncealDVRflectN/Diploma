#include "PlotField.h"
#include "files_util.h"


#pragma region Constructors

PlotField::PlotField(const PlotParams& params) : mParams(params), mPipe("gnuplot", PipeAccessType::PIPE_WRITE)
{}

#pragma endregion



#pragma region Member methods

void PlotField::plot(const std::filesystem::path& fieldData, const std::filesystem::path& fluidData)
{
    const static Vector2<int> gridSize(60, 60);

    FieldResultParams fieldParams = read_field_params(fieldData);
    FluidResultParams fluidParams = read_fluid_params(fluidData);
    std::filesystem::path fieldGridData = intermediate_file_path("field-grid-data.dat");

    mPipe.write("set term wxt size %u,%u enhanced font 'Verdana,10'\n", mParams.windowWidth, mParams.windowHeight);

    mPipe.write("set table '%s'\n", fieldGridData.string().c_str());
    mPipe.write("set dgrid3d %d,%d splines\n", gridSize.x, gridSize.y);

    mPipe.write("splot '%s' using 1:2:3\n", fieldData.string().c_str());

    mPipe.write("unset table\n");
    mPipe.write("unset dgrid3d\n");

    mPipe.write("set pm3d map interpolate 4,4\n");

    mPipe.write("load '%s'\n", plot_config_path("field.cfg").string().c_str());
    mPipe.write("set title 'Magnetic field {/Symbol c}=%lf'\n", fieldParams.chi);
    mPipe.write("set xlabel '%s' norotate textcolor rgb '#757575'\n", fieldParams.xLabel.c_str());
    mPipe.write("set ylabel '%s' norotate textcolor rgb '#757575'\n", fieldParams.yLabel.c_str());
    mPipe.write("set datafile commentschars '%c'\n", COMMENT_CHARACTER);
    mPipe.write("set cbrange [%lf:%lf]\n", fieldParams.potentialMin, fieldParams.potentialMax);

    if (mParams.isEqualAxis)
    {
        mPipe.write("set size ratio -1\n");
    }

    mPipe.write("set multiplot\n");

    mPipe.write("splot '%s' using 1:2:3 with pm3d notitle, "
                "'%s' using 1:2:(0.0) with lines notitle ls @SURFACE_STYLE\n", 
                fieldGridData.string().c_str(), 
                fluidData.string().c_str());

    mPipe.flush();
}


void PlotField::close()
{
    mPipe.close();
}

#pragma endregion