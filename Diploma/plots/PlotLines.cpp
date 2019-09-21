#include "PlotLines.h"


#pragma region Constructors

PlotLines::PlotLines(const PlotLinesParams& params) : mParams(params), mPipe(GNUPLOT, PIPE_WRITE)
{}

#pragma endregion


#pragma region Member methods

void PlotLines::plot(const std::vector<PlotLine>& lines)
{
    const size_t linesNum = lines.size();

    assert_message(linesNum > 0, "Cannot plot lines, there is no lines");

    mPipe.write("set term wxt size %d, %d enhanced\n", mParams.windowWidth, mParams.windowHeight);
    mPipe.write("set title \"%s\"\n", mParams.title.c_str());
    mPipe.write("set xlabel \"%s\"\n", mParams.labelX.c_str());
    mPipe.write("set ylabel \"%s\"\n", mParams.labelY.c_str());

    if (mParams.isEqualAxis)
    {
        mPipe.write("set size ratio -1\n");
    }

    mPipe.write("plot '-' with lines title '%s'", lines[0].title.c_str());
    for (size_t i = 1; i < linesNum; i++)
    {
        mPipe.write(", '-' with lines title '%s'", lines[i].title.c_str());
    }
    mPipe.write("\n");

    for (const PlotLine& line : lines)
    {
        const Array<Vector2<double>>& points = line.points;
        const arr_size_t pointsNum = points.size();

        for (const Vector2<double>& point : points)
        {
            mPipe.write("%lf %lf\n", point.r, point.z);
        }

        mPipe.write("e\n");
        mPipe.flush();
    }
}


void PlotLines::close()
{
    mPipe.close();
}

#pragma endregion
