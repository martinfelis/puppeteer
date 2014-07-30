#include "chartXY.h"
#include <sstream>

ChartContainer::ChartContainer( const std::string _chartTitle, const std::string _xAxisTitle, const std::string _yAxisTitle, const bool legend) : 
  timePtr(0.0),
  chartTitle(_chartTitle),
  viewPtr(vtkSmartPointer<vtkContextView>::New()),
  chartPtr(vtkSmartPointer<vtkChartXY>::New())
{

  setTitle(_chartTitle);
  chartPtr->SetAutoSize(true);
  chartPtr->SetAutoAxes(true);
  setXaxisTitle(_xAxisTitle);
  setYaxisTitle(_yAxisTitle);
  chartPtr->SetShowLegend(legend);
  viewPtr->GetScene()->AddItem(chartPtr.GetPointer());
  reset();

  vtkPlot* timeCursor = chartPtr->AddPlot(vtkChart::POINTS);
     vtkSmartPointer<vtkFloatArray> cursor_Tdata = vtkSmartPointer<vtkFloatArray>::New();
     vtkSmartPointer<vtkFloatArray> cursor_Xdata = vtkSmartPointer<vtkFloatArray>::New();
     vtkSmartPointer<vtkTable> timeCursorData = vtkSmartPointer<vtkTable>::New();

     cursor_Tdata->SetName("time-cursor");
     cursor_Xdata->SetName("now");

     timeCursorData->AddColumn(cursor_Tdata);
     timeCursorData->AddColumn(cursor_Xdata);
     timeCursorData->SetNumberOfRows(2);

     timeCursorData->SetValue(0, 0, 0.0);
     timeCursorData->SetValue(0, 1, 0.0);

     timeCursorData->SetValue(1, 0, 0.0);
     timeCursorData->SetValue(1, 1, 0.0);

#if VTK_MAJOR_VERSION <= 5
     timeCursor->SetInput(timeCursorData,0,1);
#else
     timeCursor->SetInputData(timeCursorData,0,1);
#endif

  timeCursor->SetWidth(2.5);
  timeCursor->SetColor(100, 100, 100, 100);
  vtkPlotPoints::SafeDownCast(timeCursor)->SetMarkerStyle(vtkPlotPoints::SQUARE);

  // set first plot as time pointer
  
}

void ChartContainer::registerParent(QVTKWidget* _parent) {
  viewPtr->SetInteractor(_parent->GetInteractor());
  _parent->SetRenderWindow(viewPtr->GetRenderWindow());
}

void ChartContainer::setTitle(const std::string _chartTitle) {
  chartPtr->SetTitle(_chartTitle);
}

void ChartContainer::setXaxisTitle(const std::string _title) {
  chartPtr->GetAxis(vtkAxis::BOTTOM)->SetTitle(_title);

}
void ChartContainer::setYaxisTitle(const std::string _title) {
  chartPtr->GetAxis(vtkAxis::LEFT)->SetTitle(_title);
  //  chartPtr->GetAxis(vtkAxis::RIGHT)->SetAxisVisible(true);

}

void ChartContainer::reset() {
  for (int idx = chartPtr->GetNumberOfPlots()-1; idx > 0; idx--) {
    chartPtr->RemovePlot(idx);
  }
  //chartPtr->ClearPlots();
  dataContainerVec.clear();

}
void ChartContainer::pushData(const std::string _title, const VectorNd _Tdata, const VectorNd _Xdata, const double _width,  const ChartColor _color, const int _lineType) {
  // check data integrity 
  // !! -- this should better be done with exceptions
  assert(_Tdata.size() == _Xdata.size());


  std::stringstream TdataTitleStr, XdataTitleStr;

  vtkSmartPointer<vtkFloatArray> Tdata = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> Xdata = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkTable> dataTable = vtkSmartPointer<vtkTable>::New();
  
  SplineInterpolator<VectorNd> dataContainer;
  
  TdataTitleStr << "time::" << _title;
  XdataTitleStr << "val::" << _title;
  Tdata->SetName(TdataTitleStr.str().c_str());
  Xdata->SetName(XdataTitleStr.str().c_str());

  dataTable->AddColumn(Tdata);
  dataTable->AddColumn(Xdata);
  
  dataTable->SetNumberOfRows(_Tdata.size());
  for(size_t rowIdx = 0; rowIdx < _Tdata.size(); rowIdx++) {
    dataTable->SetValue(rowIdx,0,_Tdata[rowIdx]);
    dataTable->SetValue(rowIdx,1,_Xdata[rowIdx]);
    VectorNd data = VectorNd::Zero(1);
    data[0] =  _Xdata[rowIdx];
    dataContainer.addPoints(_Tdata[rowIdx],data);
  }
  dataContainer.initialize();

  dataContainerVec.push_back(dataContainer);

  vtkPlot* plot = chartPtr->AddPlot(vtkChart::LINE);

#if VTK_MAJOR_VERSION <= 5
  plot->SetInput(dataTable, 0,1);
#else
  plot->SetInputData(dataTable, 0,1);
#endif
  plot->SetWidth(_width);
  plot->SetColor(_color.red, _color.green, _color.blue, _color.alpha);
  plot->GetPen()->SetLineType(_lineType);
 
}
void ChartContainer::update() {
  if (dataContainerVec.size() > 0) {

     vtkSmartPointer<vtkFloatArray> cursor_Tdata = vtkSmartPointer<vtkFloatArray>::New();
     vtkSmartPointer<vtkFloatArray> cursor_Xdata = vtkSmartPointer<vtkFloatArray>::New();
     vtkSmartPointer<vtkTable> timeCursorData = vtkSmartPointer<vtkTable>::New();

     cursor_Tdata->SetName("time-cursor");
     cursor_Xdata->SetName("now");

     timeCursorData->AddColumn(cursor_Tdata);
     timeCursorData->AddColumn(cursor_Xdata);

     timeCursorData->SetNumberOfRows(dataContainerVec.size());
     for (size_t idx; idx < dataContainerVec.size(); idx++) {
       SplineInterpolator<VectorNd> dataContainer = dataContainerVec[idx];
       double timeVal = timePtr;
       if (dataContainer.t_values.front() > timePtr) {
 	timeVal = dataContainer.t_values.front()+1.0e-5;
       }
       if (dataContainer.t_values.back() < timePtr) {
 	timeVal = dataContainer.t_values.back()-1.0e-5;
       }

       timeCursorData->SetValue(idx, 0, timeVal);
       timeCursorData->SetValue(idx, 1, dataContainer.getValues(timeVal)[0]);

     }


 #if VTK_MAJOR_VERSION <= 5
     chartPtr->GetPlot(0)->SetInput(timeCursorData,0,1);
 #else
     chartPtr->GetPlot(0)->SetInputData(timeCursorData,0,1);
 #endif
     viewPtr->Render();
     }
}

void ChartContainer::setTimePtr(const double _timePtr) {
  timePtr = _timePtr;
  update();
}
