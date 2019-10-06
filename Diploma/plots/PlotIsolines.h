#ifndef DIPLOMA_PLOTISOLINES_H
#define DIPLOMA_PLOTISOLINES_H


#ifndef GNUPLOT
    #define GNUPLOT "gnuplot"
#endif


#include "SimpleTriangleGrid.h"
#include "Pipe.h"
#include <string>


#pragma message("Add abstract class for grids and refactor PlotIsolines")


typedef struct plot_isolines_params_t
{
	std::string title;
	std::string labelX;
	std::string labelY;
	int windowWidth;
	int windowHeight;
	bool isEqualAxis;
} PlotIsolinesParams;


class PlotIsolines
{
public:
	PlotIsolines(const PlotIsolinesParams& params);

	void plot(const SimpleTriangleGrid& grid, const Matrix<double>& values, double volumeNonDimMul);

	void close();

private:
	Pipe mPipe;
	PlotIsolinesParams mParams;
};

#endif