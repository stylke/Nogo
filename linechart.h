#ifndef LINECHART_H
#define LINECHART_H

#include <QMainWindow>
#include <QtCharts>
#include <QVector>
using namespace QtCharts;

struct P{
    int stepCount;
    double value;
};

namespace Ui {
class LineChart;
}

class LineChart : public QMainWindow
{
    Q_OBJECT

private:
    QLineSeries *lineSeries;
    QLineSeries *HorizontalLine;
    QChart *chart;
    QChartView *chartView;
    QVector<P> points;

public:
    explicit LineChart(QWidget *parent = nullptr);
    void loadData(int stepCount,double values[]);
    ~LineChart();


private:
    Ui::LineChart *ui;
};

#endif // LINECHART_H
