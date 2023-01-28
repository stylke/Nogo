#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QSound>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "gamemodel.h"
#include "linechart.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    GameModel *game;
    int W,H; //窗口宽度和高度

    int linex; //记录行号
    int liney; //记录列号
    bool pointSelected; //是否有合适的点被选中
    int latestx; //最近一步子的行号
    int latesty; //最近一步子的列号
    int stepCount; //记录当前步数
    int steps[500][2]; //记录每一步走法，steps[i][0],[i][1]分别表示第i步落子的行号、列号
    double values[500]; //记录每一步走后，相应棋局的评估分
    bool isSaved; //记录当前棋盘状态是否保存
    int remainingTime; //记录剩余时间
    Player AIPlay; //记录AI代表的棋色
    bool randomChooseForAIPlay; //是否随机选择AI执子颜色

    QLabel * labelGameType;
    QLabel * labelSteps;
    QLabel * labelTimer;

    QTimer * timer;

    QSound * sound;
    bool canPlayMusic; //是否开启音效

    QMediaPlayer * player;
    QMediaPlaylist * playlist;

    LineChart * lineChart;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void timeLimitExceeded();

protected:
    //继承自QWidget的事件
    void paintEvent(QPaintEvent *event) override; //调用update()时触发
    void mouseMoveEvent(QMouseEvent *event) override; //鼠标移动时触发
    void mouseReleaseEvent(QMouseEvent *event) override; //鼠标释放时触发
    void closeEvent(QCloseEvent *event) override; //窗口被关闭时触发
    void keyPressEvent(QKeyEvent *event) override;//按下按键时触发

private slots:
    void on_actionPVP_triggered();

    void on_actionPVE_triggered();

    void on_actionSave_triggered();

    void on_actionrestore_last_saved_board_triggered();

    void on_actionShowGameRules_triggered();

    void on_actionRetract_triggered();

    void on_actionShowLosingPoints_triggered(bool checked);

    void on_actionShowGameInfo_triggered();

    void on_timeOut(); //用于手动连接的槽函数，响应计时器发出的timeout信号

    void on_actionSettings_triggered();

    void on_actionShowEvaluation_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
