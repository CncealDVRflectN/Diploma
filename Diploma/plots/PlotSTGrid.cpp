#include "PlotSTGrid.h"


#pragma region Constructors

PlotSTGrid::PlotSTGrid(const PlotSTGridParams& params) : mParams(params), mPipe(GNUPLOT, PIPE_WRITE)
{}

#pragma endregion


#pragma region Member methods

void PlotSTGrid::plot(const SimpleTriangleGrid& grid)
{
    const Matrix<Vector2<double>> points = mParams.volumeNonDimMul * grid.rawPoints();

    mPipe.write("set term wxt size %d, %d enhanced\n", mParams.windowWidth, mParams.windowHeight);
    mPipe.write("set title \"%s\"\n", mParams.title.c_str());
    mPipe.write("set xlabel \"%s\"\n", mParams.labelX.c_str());
    mPipe.write("set ylabel \"%s\"\n", mParams.labelY.c_str());

    if (mParams.isEqualAxis)
    {
        mPipe.write("set size ratio -1\n");
    }

    mPipe.write("plot '-' with lines title \"¬нутренн€€ сетка\" lc rgb \"red\", "
                "'-' with lines title \"¬нешн€€ сетка\" lc rgb \"blue\"\n");

    const arr_size_t gridRowsNum = grid.rowsNum();
    const arr_size_t gridColumnsNum = grid.columnsNum();
    const arr_size_t surfaceColumnIndex = grid.surfaceColumnsIndex();

    for (arr_size_t i = 0; i < gridRowsNum; i++)
    {
        for (arr_size_t j = 0; j <= surfaceColumnIndex; j++)
        {
            mPipe.write("%lf %lf\n", points(i, j).r, points(i, j).z);

            if (i != gridRowsNum - 1 && j != surfaceColumnIndex)
            {
                mPipe.write("%lf %lf\n", points(i + 1, j).r, points(i + 1, j).z);
                mPipe.write("%lf %lf\n", points(i, j + 1).r, points(i, j + 1).z);
                mPipe.write("%lf %lf\n", points(i, j).r, points(i, j).z);
            }
            else if (i != gridRowsNum - 1)
            {
                for (arr_size_t k = surfaceColumnIndex; k >= 0; k--)
                {
                    mPipe.write("%lf %lf\n", points(i + 1, k).r, points(i + 1, k).z);
                }
            }
        }
    }

    mPipe.write("e\n");
    mPipe.flush();

    for (arr_size_t i = 0; i < gridRowsNum; i++)
    {
        for (arr_size_t j = surfaceColumnIndex; j < gridColumnsNum; j++)
        {
            mPipe.write("%lf %lf\n", points(i, j).r, points(i, j).z);

            if (i != gridRowsNum - 1 && j != gridColumnsNum - 1)
            {
                mPipe.write("%lf %lf\n", points(i + 1, j).r, points(i + 1, j).z);
                mPipe.write("%lf %lf\n", points(i, j + 1).r, points(i, j + 1).z);
                mPipe.write("%lf %lf\n", points(i, j).r, points(i, j).z);
            }
            else if (i != gridRowsNum - 1)
            {
                for (arr_size_t k = gridColumnsNum - 1; k >= surfaceColumnIndex; k--)
                {
                    mPipe.write("%lf %lf\n", points(i + 1, k).r, points(i + 1, k).z);
                }
            }
        }
    }

    mPipe.write("e\n");
    mPipe.flush();
}


void PlotSTGrid::close()
{
    mPipe.close();
}

#pragma endregion
