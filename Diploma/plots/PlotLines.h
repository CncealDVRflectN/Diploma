#ifndef DIPLOMA_PLOTLINES_H
#define DIPLOMA_PLOTLINES_H


#ifndef GNUPLOT
    #define GNUPLOT "gnuplot"
#endif


#include "Plot.h"
#include "Array.h"
#include "Pipe.h"
#include <vector>
#include <string>


struct plot_line_t
{
    std::string title;
    vtkColor4ub color;
    Array<Vector2<double>> points;
    double width;


    plot_line_t(const Array<Vector2<double>>& linePoints) : points(linePoints), 
                                                            title(), 
                                                            color(0.0, 0.0, 0.0, 0.0), 
                                                            width(1.0)
    {}
};


struct plot_lines_params_t : public plot_params_t
{
    std::string labelX;
    std::string labelY;
    bool isEqualAxis;
};


using PlotLine = plot_line_t;
using PlotLinesParams = plot_lines_params_t;


void plot(const PlotLinesParams& plotParams, const std::vector<PlotLine>& lines);

#endif