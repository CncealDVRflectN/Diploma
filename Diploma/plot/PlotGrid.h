#ifndef DIPLOMA_PLOTGRID_H
#define DIPLOMA_PLOTGRID_H

#include "../util/util.h"
#include <string>


typedef struct plot_grid_params_t
{
	std::string title;
	std::string labelX;
	std::string labelY;
	int windowWidth;
	int windowHeight;
	double volumeNonDimMul;
	bool isEqualAxis;
} PlotGridParams;


class PlotGrid
{
public:
	PlotGrid(const PlotGridParams& plotParams);

	~PlotGrid();

	void setGrid(Vector2** grid, int linesNum, int columnsNum, int surfaceColumnIndex);

	void plot();

	void close();

private:
	Vector2** grid;
	int gridLinesNum;
	int gridColumnsNum;
	int surfaceColumnIndex;

	PlotGridParams params;

	FILE* pipe;
};

#endif