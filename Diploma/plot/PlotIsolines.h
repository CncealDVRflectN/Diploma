#ifndef DIPLOMA_PLOTISOLINES_H
#define DIPLOMA_PLOTISOLINES_H

#include "../util/util.h"
#include <string>


typedef struct plot_isolines_params_t
{
	std::string title;
	std::string labelX;
	std::string labelY;
	int windowWidth;
	int windowHeight;
	double volumeNonDimMul;
	bool isEqualAxis;
} PlotIsolinesParams;


class PlotIsolines
{
//public:
//	PlotIsolines(const PlotIsolinesParams& plotParams);
//
//	~PlotIsolines();
//
//	void setGridAndValues(Vector2** grid, double** values, int linesNum, int columnsNum, int surfaceColumnIndex);
//
//	void plot();
//
//	void close();
//
//private:
//	Vector2** grid;
//	double** values;
//	int gridLinesNum;
//	int gridColumnsNum;
//	int surfaceColumnIndex;
//
//	PlotIsolinesParams params;
//
//	FILE* pipe;
};

#endif