#include "linechart.h"
#include "ui_linechart.h"
#include <QDebug>

LineChart::LineChart(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LineChart)
{
    ui->setupUi(this);
    setWindowTitle("局势评估");

    lineSeries = new QLineSeries;

    HorizontalLine = new QLineSeries;

    chart = new QChart;
    chart->addSeries(lineSeries);
    chart->addSeries(HorizontalLine);

    chartView = new QChartView(chart);

    setCentralWidget(chartView);
}

void LineChart::loadData(int stepCount, double values[])
{
    chart->removeSeries(lineSeries);
    chart->removeSeries(HorizontalLine);
    lineSeries->clear();
    HorizontalLine->clear();

    for(int i = 1;i <= stepCount;i++){
        lineSeries->append(i,values[i]);
    }
    HorizontalLine->append(0,0);
    HorizontalLine->append(stepCount,0);

    chart->legend()->hide();
    chart->addSeries(lineSeries);
    chart->addSeries(HorizontalLine);
    chart->createDefaultAxes();
    chart->setTitle("局势评估(PVE模式下，y值越大，AI越占优；PVP模式下，y值越大，黑棋越占优)");

    chartView->setRenderHint(QPainter::Antialiasing);

    setCentralWidget(chartView);
}



LineChart::~LineChart()
{
    delete ui;
}

