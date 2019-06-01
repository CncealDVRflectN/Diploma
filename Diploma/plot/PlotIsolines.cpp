#include "PlotIsolines.h"

#ifndef GNUPLOT
#define GNUPLOT "gnuplot"
#endif


PlotIsolines::PlotIsolines(const PlotIsolinesParams& plotParams)
{
	this->params = plotParams;
	this->gridLinesNum = 0;
	this->gridColumnsNum = 0;
	this->surfaceColumnIndex = 0;
}


PlotIsolines::~PlotIsolines()
{
	for (int i = 0; i < gridLinesNum; i++)
	{
		delete grid[i];
		delete values[i];
	}

	delete grid;
	delete values;
}


void PlotIsolines::setGridAndValues(Vector2** grid, double** values, int linesNum, int columnsNum, int surfaceColumnIndex)
{
	if (gridLinesNum != 0 && gridColumnsNum != 0)
	{
		for (int i = 0; i < gridLinesNum; i++)
		{
			delete this->grid[i];
			delete this->values[i];
		}

		delete this->grid;
		delete this->values;

		gridLinesNum = 0;
		gridColumnsNum = 0;
		surfaceColumnIndex = 0;
	}

	this->grid = new Vector2*[linesNum];
	this->values = new double*[linesNum];

	for (int i = 0; i < linesNum; i++)
	{
		this->grid[i] = new Vector2[columnsNum];
		this->values[i] = new double[columnsNum];

		for (int j = 0; j < columnsNum; j++)
		{
			this->grid[i][j] = { params.volumeNonDimMul * grid[i][j].r, params.volumeNonDimMul * grid[i][j].z };
			this->values[i][j] = values[i][j];
		}
	}

	this->gridLinesNum = linesNum;
	this->gridColumnsNum = columnsNum;
	this->surfaceColumnIndex = surfaceColumnIndex;
}


void PlotIsolines::plot()
{
#ifdef WIN32
	pipe = _popen(GNUPLOT, "w");
#else
	pipe = popen(GNUPLOT, "w");
#endif

	if (grid == nullptr || gridLinesNum == 0 || gridColumnsNum == 0)
	{
		this->close();

		return;
	}

	fprintf(pipe, "set term wxt enhanced size %d, %d\n", params.windowWidth, params.windowHeight);
	fprintf(pipe, "set title \"%s\"\n", params.title.c_str());
	fprintf(pipe, "set xlabel \"%s\"\n", params.labelX.c_str());
	fprintf(pipe, "set ylabel \"%s\"\n", params.labelY.c_str());
	fprintf(pipe, "set dgrid3d splines\n");
	fprintf(pipe, "set contour base\n");
	fprintf(pipe, "set cntrparam levels discrete 0.00001, 0.0001, 0.001, 0.01, 0.05, 0.1, 0.3, 0.7, 1.3, 2.0, ");
	fprintf(pipe, "3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0\n");
	fprintf(pipe, "set cntrparam bspline\n");
	fprintf(pipe, "set cntrparam order 10\n");
	fprintf(pipe, "set cntrlabel font \",7\"\n");
	fprintf(pipe, "set view map\n");
	fprintf(pipe, "unset surface\n");

	if (params.isEqualAxis)
	{
		fprintf(pipe, "set size ratio -1\n");
	}

	fprintf(pipe, "splot '-' with lines notitle, '-' with labels boxed notitle\n");

	for (int i = 0; i < gridLinesNum; i++)
	{
		for (int j = 0; j < gridColumnsNum; j++)
		{
			fprintf(pipe, "%lf %lf %lf\n", grid[i][j].r, grid[i][j].z, values[i][j]);
		}
	}

	fprintf(pipe, "e\n");
	fflush(pipe);

	for (int i = 0; i < gridLinesNum; i++)
	{
		for (int j = 0; j < gridColumnsNum; j++)
		{
			fprintf(pipe, "%lf %lf %lf\n", grid[i][j].r, grid[i][j].z, values[i][j]);
		}
	}

	fprintf(pipe, "e\n");
	fflush(pipe);
}


void PlotIsolines::close()
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