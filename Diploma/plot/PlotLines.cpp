#include "PlotLines.h"

#ifndef GNUPLOT
	#define GNUPLOT "gnuplot"
#endif


PlotLines::PlotLines(const PlotLinesParams& params)
{
	this->params = params;
}


#pragma mark Public methods

void PlotLines::addLine(PlotLine& line)
{
	lines.insert(lines.end(), line);
}


void PlotLines::clear()
{
	lines.clear();
}


void PlotLines::plot()
{
#ifdef WIN32
	pipe = _popen(GNUPLOT, "w");
#else
	pipe = popen(GNUPLOT, "w");
#endif

	auto linesNum = lines.size();

	if (linesNum == 0)
	{
		this->close();

		return;
	}

	fprintf(pipe, "set term wxt size %d, %d\n", params.windowWidth, params.windowHeight);
	fprintf(pipe, "set title \"%s\"\n", params.title.c_str());
	fprintf(pipe, "set xlabel \"%s\"\n", params.labelX.c_str());
	fprintf(pipe, "set ylabel \"%s\"\n", params.labelY.c_str());

	if (params.isEqualAxis)
	{
		fprintf(pipe, "set size ratio -1\n");
	}

	fprintf(pipe, "plot '-' with lines title '%s'", lines[0].title.c_str());
	for (auto i = 1; i < linesNum; i++)
	{
		fprintf(pipe, ", '-' with lines title '%s'", lines[i].title.c_str());
	}
	fprintf(pipe, "\n");

	for (const PlotLine& line : lines)
	{
		for (const Vector2& point : line.points)
		{
			fprintf(pipe, "%lf %lf\n", point.r, point.z);
		}
		fprintf(pipe, "e\n");
		fflush(pipe);
	}
}


void PlotLines::close()
{
	if (pipe != nullptr)
	{
#ifdef WIN32
		_pclose(pipe);
#else
		pclose(pipe);
#endif

		pipe = nullptr;
	}
}