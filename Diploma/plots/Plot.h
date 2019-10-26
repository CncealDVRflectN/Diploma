#ifndef DIPLOMA_PLOT_H
#define DIPLOMA_PLOT_H

#include <string>
#include <vtkColor.h>


typedef struct plot_params_t
{
    std::string title;
    vtkColor3f backgroundColor;
    unsigned int windowWidth;
    unsigned int windowHeight;
    unsigned int titleFontSize;
    unsigned int axisLabelsFontSize;
} PlotParams;

#endif