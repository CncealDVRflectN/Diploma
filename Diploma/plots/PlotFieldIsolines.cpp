#include <sstream>
#include "PlotFieldIsolines.h"
#include "BezierCurve.h"
#include "files_util.h"


const BezierCurve INTERNAL_DISTRIBUTION({ {0.0, 0.0}, {0.45, 0.6}, {1.0, 1.0} });
const BezierCurve EXTERNAL_DISTRIBUTION({ {0.0, 0.0}, {0.3, 0.7}, {1.0, 1.0} });

const int INTERNAL_CONTOURS_NUM = 3;
const int EXTERNAL_CONTOURS_NUM = 6;


#pragma region Constructors

PlotFieldIsolines::PlotFieldIsolines(const PlotParams& params) : mParams(params), mPipe("gnuplot", PipeAccessType::PIPE_WRITE)
{}

#pragma endregion


#pragma region Member methods

void PlotFieldIsolines::plot(const std::filesystem::path& fieldData, const std::filesystem::path& fluidData)
{
    const static Vector2<int> gridSize(60, 60);

    FieldResultParams fieldParams = read_field_params(fieldData);
    FluidResultParams fluidParams = read_fluid_params(fluidData);
    std::filesystem::path contoursData = intermediate_file_path("field-contours.dat");
    std::string internalContours = generateInternalContours(fieldParams.potentialMin, fieldParams.fluidTopPotential);
    std::string externalContours = generateExternalContours(fieldParams.potentialMax, fieldParams.fluidTopPotential);


    mPipe.write("set term wxt size %u,%u enhanced font 'Verdana,10'\n", mParams.windowWidth, mParams.windowHeight);

    mPipe.write("set table '%s'\n", contoursData.string().c_str());
    mPipe.write("set dgrid3d %d,%d splines\n", gridSize.x, gridSize.y);
    mPipe.write("set contour base\n");
    mPipe.write("set cntrparam linear\n");
    mPipe.write("set cntrparam levels discrete %s,%lf,%s\n", internalContours.c_str(),
                                                             fieldParams.fluidTopPotential,
                                                             externalContours.c_str());
    mPipe.write("set view map\n");
    mPipe.write("unset surface\n");

    mPipe.write("splot '%s' using 1:2:3\n", fieldData.string().c_str());

    mPipe.write("unset table\n");
    mPipe.write("unset dgrid3d\n");
    mPipe.write("unset contour\n");

    mPipe.write("load '%s'\n", plot_config_path("field-isolines.cfg").string().c_str());
    mPipe.write("set title 'Field isolines {/Symbol c}=%lf'\n", fieldParams.chi);
    mPipe.write("set xlabel '%s' norotate textcolor rgb '#757575'\n", fieldParams.xLabel.c_str());
    mPipe.write("set ylabel '%s' norotate textcolor rgb '#757575'\n", fieldParams.yLabel.c_str());
    mPipe.write("set datafile commentschars '%c'\n", COMMENT_CHARACTER);
    mPipe.write("set cntrparam levels discrete %s,%lf,%s\n", internalContours.c_str(), 
                                                             fieldParams.fluidTopPotential, 
                                                             externalContours.c_str());

    if (mParams.isEqualAxis)
    {
        mPipe.write("set size ratio -1\n");
    }

    mPipe.write("plot '%s' with lines notitle palette, "
                "'%s' using 1:2:(sprintf('%%.2f', $3)) every 37 with labels boxed notitle, "
                "'%s' using 1:2 with lines notitle ls @SURFACE_STYLE\n", 
                contoursData.string().c_str(),
                contoursData.string().c_str(),
                fluidData.string().c_str(), 
                fluidParams.w);

    mPipe.flush();
}


std::string PlotFieldIsolines::generateInternalContours(double startVal, double endVal) const
{
    std::stringstream result;
    double step = 1.0 / (INTERNAL_CONTOURS_NUM + 1);
    double dif = endVal - startVal;
    
    for (int i = 1; i <= INTERNAL_CONTOURS_NUM; i++)
    {
        result << startVal + INTERNAL_DISTRIBUTION.evaluate(i * step).y * dif;

        if (i < INTERNAL_CONTOURS_NUM)
        {
            result << ',';
        }
    }

    return result.str();
}


std::string PlotFieldIsolines::generateExternalContours(double startVal, double endVal) const
{
    std::stringstream result;
    double step = 1.0 / (EXTERNAL_CONTOURS_NUM + 1);
    double dif = endVal - startVal;

    for (int i = 1; i <= EXTERNAL_CONTOURS_NUM; i++)
    {
        result << startVal + EXTERNAL_DISTRIBUTION.evaluate(i * step).y * dif;

        if (i < EXTERNAL_CONTOURS_NUM)
        {
            result << ',';
        }
    }

    return result.str();
}


void PlotFieldIsolines::close()
{
    mPipe.close();
}

#pragma endregion