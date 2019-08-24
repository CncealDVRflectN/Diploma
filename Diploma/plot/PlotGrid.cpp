#include "PlotGrid.h"

#ifndef GNUPLOT
#define GNUPLOT "gnuplot"
#endif


//PlotGrid::PlotGrid(const PlotGridParams& plotParams)
//{
//	this->params = plotParams;
//	this->gridLinesNum = 0;
//	this->gridColumnsNum = 0;
//	this->surfaceColumnIndex = 0;
//}
//
//
//PlotGrid::~PlotGrid()
//{
//	for (int i = 0; i < gridLinesNum; i++)
//	{
//		delete grid[i];
//	}
//
//	delete grid;
//}
//
//
//void PlotGrid::setGrid(Vector2** grid, int linesNum, int columnsNum, int surfaceColumnIndex)
//{
//	if (gridLinesNum != 0 && gridColumnsNum != 0)
//	{
//		for (int i = 0; i < gridLinesNum; i++)
//		{
//			delete this->grid[i];
//		}
//
//		delete this->grid;
//
//		gridLinesNum = 0;
//		gridColumnsNum = 0;
//		surfaceColumnIndex = 0;
//	}
//
//	this->grid = new Vector2*[linesNum];
//
//	for (int i = 0; i < linesNum; i++)
//	{
//		this->grid[i] = new Vector2[columnsNum];
//
//		for (int j = 0; j < columnsNum; j++)
//		{
//			this->grid[i][j] = { params.volumeNonDimMul * grid[i][j].r, params.volumeNonDimMul * grid[i][j].z };
//		}
//	}
//
//	this->gridLinesNum = linesNum;
//	this->gridColumnsNum = columnsNum;
//	this->surfaceColumnIndex = surfaceColumnIndex;
//}
//
//
//void PlotGrid::plot()
//{
//#ifdef WIN32
//	pipe = _popen(GNUPLOT, "w");
//#else
//	pipe = popen(GNUPLOT, "w");
//#endif
//
//	if (grid == nullptr || gridLinesNum == 0 || gridColumnsNum == 0)
//	{
//		this->close();
//
//		return;
//	}
//
//	fprintf(pipe, "set term wxt size %d, %d enhanced\n", params.windowWidth, params.windowHeight);
//	fprintf(pipe, "set title \"%s\"\n", params.title.c_str());
//	fprintf(pipe, "set xlabel \"%s\"\n", params.labelX.c_str());
//	fprintf(pipe, "set ylabel \"%s\"\n", params.labelY.c_str());
//
//	if (params.isEqualAxis)
//	{
//		fprintf(pipe, "set size ratio -1\n");
//	}
//
//	fprintf(pipe, "plot '-' with lines title \"¬нутренн€€ сетка\" lc rgb \"red\", '-' with lines title \"¬нешн€€ сетка\" lc rgb \"blue\"\n");
//
//	for (int i = 0; i < gridLinesNum; i++)
//	{
//		for (int j = 0; j <= surfaceColumnIndex; j++)
//		{
//			fprintf(pipe, "%lf %lf\n", grid[i][j].r, grid[i][j].z);
//
//			if (i != gridLinesNum - 1 && j != surfaceColumnIndex)
//			{
//				fprintf(pipe, "%lf %lf\n", grid[i + 1][j].r, grid[i + 1][j].z);
//				fprintf(pipe, "%lf %lf\n", grid[i][j + 1].r, grid[i][j + 1].z);
//				fprintf(pipe, "%lf %lf\n", grid[i][j].r, grid[i][j].z);
//			}
//			else if (i != gridLinesNum - 1)
//			{
//				for (int k = surfaceColumnIndex; k >= 0; k--)
//				{
//					fprintf(pipe, "%lf %lf\n", grid[i + 1][k].r, grid[i + 1][k].z);
//				}
//			}
//		}
//	}
//
//	fprintf(pipe, "e\n");
//	fflush(pipe);
//
//	for (int i = 0; i < gridLinesNum; i++)
//	{
//		for (int j = surfaceColumnIndex; j < gridColumnsNum; j++)
//		{
//			fprintf(pipe, "%lf %lf\n", grid[i][j].r, grid[i][j].z);
//
//			if (i != gridLinesNum - 1 && j != gridColumnsNum - 1)
//			{
//				fprintf(pipe, "%lf %lf\n", grid[i + 1][j].r, grid[i + 1][j].z);
//				fprintf(pipe, "%lf %lf\n", grid[i][j + 1].r, grid[i][j + 1].z);
//				fprintf(pipe, "%lf %lf\n", grid[i][j].r, grid[i][j].z);
//			}
//			else if (i != gridLinesNum - 1)
//			{
//				for (int k = gridColumnsNum - 1; k >= surfaceColumnIndex; k--)
//				{
//					fprintf(pipe, "%lf %lf\n", grid[i + 1][k].r, grid[i + 1][k].z);
//				}
//			}
//		}
//	}
//
//	fprintf(pipe, "e\n");
//	fflush(pipe);
//}
//
//
//void PlotGrid::close()
//{
//	if (pipe != nullptr)
//	{
//#ifdef WIN32
//		_pclose(pipe);
//#else
//		pclose(pipe);
//#endif
//
//		pipe = nullptr;
//	}
//}