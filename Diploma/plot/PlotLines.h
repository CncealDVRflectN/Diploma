#ifndef DIPLOMA_PLOTLINES_H
#define DIPLOMA_PLOTLINES_H

#include <vector>
#include "../util/util.h"


typedef struct plot_line_t
{
	std::vector<Vector2> points;
	std::string title;
} PlotLine;


typedef struct plot_lines_params_t
{
	std::string title;
	std::string labelX;
	std::string labelY;
	int windowWidth;
	int windowHeight;
	bool isEqualAxis;
} PlotLinesParams;


class PlotLines
{
public:
	PlotLines(const PlotLinesParams& plotParams);

	void addLine(PlotLine& line);

	void clear();

	void plot();

	void close();

private:
	std::vector<PlotLine> lines;
	PlotLinesParams params;
	FILE* pipe;
};

#endif