#ifndef DIPLOMA_PLOTSTGRID_H
#define DIPLOMA_PLOTSTGRID_H


#ifndef GNUPLOT
    #define GNUPLOT "gnuplot"
#endif


#include "Pipe.h"
#include "SimpleTriangleGrid.h"
#include <string>


typedef struct plot_st_grid_params_t
{
	std::string title;
	std::string labelX;
	std::string labelY;
	int windowWidth;
	int windowHeight;
	double volumeNonDimMul;
	bool isEqualAxis;
} PlotSTGridParams;


class PlotSTGrid
{
public:
	PlotSTGrid(const PlotSTGridParams& params);


	void plot(const SimpleTriangleGrid& grid);

	void close();

private:
    Pipe mPipe;
	PlotSTGridParams mParams;
};

#endif