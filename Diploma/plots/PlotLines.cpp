#include "PlotLines.h"
#include <vtkMeta.h>
#include <vtkFloatArray.h>
#include <vtkTable.h>
#include <vtkContextView.h>
#include <vtkChartXY.h>
#include <vtkPlot.h>
#include <vtkRenderer.h>
#include <vtkContextScene.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkAxis.h>
#include <vtkTextProperty.h>
#include <vtkStdString.h>
#include <vtkFXAAOptions.h>


vtkSmartPointer<vtkContextView> setupContextView(const PlotLinesParams& plotParams)
{
    vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();

    vtkSmartPointer<vtkRenderWindow> window = view->GetRenderWindow();
    window->SetSize(plotParams.windowWidth, plotParams.windowHeight);
    window->SetMultiSamples(8);

    vtkSmartPointer<vtkRenderer> renderer = view->GetRenderer();
    renderer->SetBackground(plotParams.backgroundColor.GetRed(), 
                            plotParams.backgroundColor.GetGreen(), 
                            plotParams.backgroundColor.GetBlue());
    
    return view;
}


void setupAxis(vtkSmartPointer<vtkAxis> axis, const PlotLinesParams& plotParams, const std::string& title)
{
    vtkSmartPointer<vtkTextProperty> textProperties = axis->GetTitleProperties();
    textProperties->SetFontSize(plotParams.axisLabelsFontSize);

    axis->SetTitle(title);
}


vtkSmartPointer<vtkChartXY> setupChart(const PlotLinesParams& plotParams)
{
    vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
    chart->SetTitle(plotParams.title);

    vtkSmartPointer<vtkTextProperty> titleTextProperties = chart->GetTitleProperties();
    titleTextProperties->SetFontSize(plotParams.titleFontSize);

    setupAxis(chart->GetAxis(vtkAxis::BOTTOM), plotParams, plotParams.labelX);
    setupAxis(chart->GetAxis(vtkAxis::LEFT), plotParams, plotParams.labelY);

    return chart;
}


void setupData(vtkSmartPointer<vtkChartXY> chart, const std::vector<PlotLine>& lines)
{
    for (const PlotLine& line : lines)
    {
        vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

        vtkSmartPointer<vtkFloatArray> arrX = vtkSmartPointer<vtkFloatArray>::New();
        arrX->SetName("X");
        table->AddColumn(arrX);

        vtkSmartPointer<vtkFloatArray> arrY = vtkSmartPointer<vtkFloatArray>::New();
        arrY->SetName("Y");
        table->AddColumn(arrY);

        table->SetNumberOfRows(line.points.size());

        int rowIndex = 0;
        for (const Vector2<double>& point : line.points)
        {
            table->SetValue(rowIndex, 0, point.x);
            table->SetValue(rowIndex, 1, point.y);
            rowIndex++;
        }

        vtkPlot* linePlot = chart->AddPlot(vtkChart::LINE);
        linePlot->SetInputData(table, 0, 1);
        linePlot->SetColor(line.color.GetRed(), line.color.GetGreen(), line.color.GetBlue(), line.color.GetAlpha());
        linePlot->SetWidth(line.width);
        linePlot->SetLabel(line.title);
    }
}


void plot(const PlotLinesParams& plotParams, const std::vector<PlotLine>& lines)
{
    vtkSmartPointer<vtkContextView> view = setupContextView(plotParams);
    vtkSmartPointer<vtkChartXY> chart = setupChart(plotParams);
    view->GetScene()->AddItem(chart);

    setupData(chart, lines);

    view->GetRenderWindow()->Render();
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();
}
