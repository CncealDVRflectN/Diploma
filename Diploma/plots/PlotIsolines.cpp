#include "PlotIsolines.h"


#pragma region Constructors

PlotIsolines::PlotIsolines(const PlotIsolinesParams& params) : mParams(params), mPipe(GNUPLOT, PIPE_WRITE)
{}

#pragma endregion


#pragma region Member methods

void PlotIsolines::plot(const SimpleTriangleGrid& grid, const Matrix<double>& values)
{
    const arr_size_t rowsNum = grid.rowsNum();
    const arr_size_t columnsNum = grid.columnsNum();

    assert_message(rowsNum == values.rowsNum() && columnsNum == values.columnsNum(), 
                   "Cannot plot isolines due to different dimensions of grid and values matrices");
    assert_message(rowsNum != 0 && columnsNum != 0, "Cannot plot isoline due to empty data");

    const Matrix<Vector2<double>> points = mParams.volumeNonDimMul * grid.rawPoints();

    mPipe.write("set term wxt enhanced size %d, %d\n", mParams.windowWidth, mParams.windowHeight);
    mPipe.write("set title \"%s\"\n", mParams.title.c_str());
    mPipe.write("set xlabel \"%s\"\n", mParams.labelX.c_str());
    mPipe.write("set ylabel \"%s\"\n", mParams.labelY.c_str());
    mPipe.write("set dgrid3d splines\n");
    mPipe.write("set contour base\n");
    mPipe.write("set cntrparam levels discrete 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, "
                "0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.0, 1.05, 1.1, "
                "1.15, 1.2, 1.25, 1.3, 1.35, 1.4, 1.45, 1.5, 1.75, 2.0, 2.25, 2.5, 2.75, 3.0, "
                "3.25, 3.5, 3.75, 4.0\n");
    mPipe.write("set cntrparam bspline\n");
    mPipe.write("set cntrparam order 10\n");
    mPipe.write("set cntrlabel font \",7\"\n");
    mPipe.write("set view map\n");
    mPipe.write("unset surface\n");

    if (mParams.isEqualAxis)
    {
        mPipe.write("set size ratio -1\n");
    }

    mPipe.write("splot '-' with lines notitle, '-' with labels boxed notitle\n");

    for (arr_size_t i = 0; i < rowsNum; i++)
    {
        for (arr_size_t j = 0; j < columnsNum; j++)
        {
            mPipe.write("%lf %lf %lf\n", points(i, j).r, points(i, j).z, values(i, j));
        }
    }

    mPipe.write("e\n");
    mPipe.flush();

    for (arr_size_t i = 0; i < rowsNum; i++)
    {
        for (arr_size_t j = 0; j < columnsNum; j++)
        {
            mPipe.write("%lf %lf %lf\n", points(i, j).r, points(i, j).z, values(i, j));
        }
    }

    mPipe.write("e\n");
    mPipe.flush();
}


void PlotIsolines::close()
{
    mPipe.close();
}

#pragma endregion
