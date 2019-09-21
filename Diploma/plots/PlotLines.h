#ifndef DIPLOMA_PLOTLINES_H
#define DIPLOMA_PLOTLINES_H


#ifndef GNUPLOT
    #define GNUPLOT "gnuplot"
#endif


#include "Array.h"
#include "Pipe.h"
#include <vector>
#include <string>


typedef struct plot_line_t
{
    std::string title;
	Array<Vector2<double>> points;
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
	PlotLines(const PlotLinesParams& params);


	void plot(const std::vector<PlotLine>& lines);

	void close();

private:
	Pipe mPipe;
	PlotLinesParams mParams;
};

#endif