#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include "dialogsettings.h"
#include <string>
#include <ctime>
#include <cstdlib>
using std::string;
using std::srand;
using std::rand;
using std::time;

extern int TIME_LIMIT;
extern int LINE_NUM;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("不围棋"); //设置窗口标题

    //利用gamemodel里定义的常量计算窗口大小
    W = MARGIN * 2 + (LINE_NUM - 1) * LINE_DIST;
    H = MARGIN * 2 + (LINE_NUM - 1) * LINE_DIST;
    setFixedSize(W,H); //为窗口设置固定大小
    linex = -1; //默认-1，即非合法位置
    liney = -1;
    pointSelected = false;
    latestx = -1;
    latesty = -1;
    stepCount = 0;
    isSaved = true;
    remainingTime = TIME_LIMIT;
    AIPlay = Black;
    randomChooseForAIPlay = true;

    setMouseTracking(true); //让窗口始终跟踪鼠标位置

    timer = new QTimer;
    timer->setInterval(1000);//设置timer发送timeout信号的时间间隔为1000ms
    timer->start();

    on_actionShowGameInfo_triggered(); //游戏开始时，呼出提示窗口

    game = new GameModel;

    labelGameType = new QLabel;
    if(game->getType() == PVP)
        labelGameType->setText("当前模式：PVP");
    else if(game->getType() == PVE)
        labelGameType->setText("当前模式：PVE");
    ui->statusbar->addWidget(labelGameType);

    labelSteps = new QLabel;
    labelSteps->setText(QString::asprintf("    当前步数:%d",stepCount));
    ui->statusbar->addWidget(labelSteps);

    labelTimer = new QLabel;
    labelTimer->setText(QString::asprintf("    剩余时间:%d秒",remainingTime));
    ui->statusbar->addWidget(labelTimer);

    //落子提示音
    sound = new QSound(":/res/music/placechess.wav",this); //此处用的是文件path(在左边栏右键选中该文件，点“Copy Path”)
    canPlayMusic = true; //默认开启音效

    //背景音乐
    player = new QMediaPlayer;
    playlist = new QMediaPlaylist;
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(playlist);
    playlist->addMedia(QUrl("qrc:/res/music/bgm.mp3")); //此处用的是文件url(在左边栏右键选中该文件，点“Copy URL”)
    playlist->setCurrentIndex(0);
    player->play();

    connect(timer,SIGNAL(timeout()),this,SLOT(on_timeOut()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); //设置抗锯齿

    //画背景图片
    QPixmap pixmap(":/res2/images/background.png");
    painter.drawPixmap(this->rect(),pixmap);

    //画棋盘
    for(int i = 0;i < LINE_NUM;i++){
        //画竖线
        painter.drawLine(MARGIN + i * LINE_DIST,MARGIN,MARGIN + i * LINE_DIST,MARGIN + (LINE_NUM - 1) * LINE_DIST);
        //画横线
        painter.drawLine(MARGIN,MARGIN + i * LINE_DIST,MARGIN + (LINE_NUM - 1) * LINE_DIST,MARGIN + i * LINE_DIST);
    }

    //画提示点
    if(pointSelected){
        //设置并应用brush
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        if(game->getPlayer() == Black) brush.setColor(Qt::black);
        else brush.setColor(Qt::white);
        painter.setBrush(brush);
        //计算出应画点的位置
        int centerx = MARGIN + (linex - 1) * LINE_DIST;
        int centery = MARGIN + (liney - 1) * LINE_DIST;
        QPoint center(centerx,centery);
        //画点
        painter.drawEllipse(center,DOT_DIAM/2,DOT_DIAM/2);
    }

    //画棋子
    //画棋子应遍历整个棋盘，为有子点画上棋子，不能像上面画提示点那样画，否则下一次刷新时，棋子就没了
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    for(int i = 1;i <= LINE_NUM;i++){
        for(int j = 1;j <= LINE_NUM;j++){
            int centerx = MARGIN + (i - 1) * LINE_DIST;
            int centery = MARGIN + (j - 1) * LINE_DIST;
            QPoint center(centerx,centery);
            if(game->getBoard(i,j) == 1){
                brush.setColor(Qt::black);
                painter.setBrush(brush);
                painter.drawEllipse(center,CHESS_DIAM/2,CHESS_DIAM/2);
            }else if(game->getBoard(i,j) == 2){
                brush.setColor(Qt::white);
                painter.setBrush(brush);
                painter.drawEllipse(center,CHESS_DIAM/2,CHESS_DIAM/2);
            }
        }
    }

    //画最后一步棋的提示红点
    if(latestx != -1){
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(Qt::red);
        painter.setBrush(brush);
        int centerx = MARGIN + (latestx - 1) * LINE_DIST;
        int centery = MARGIN + (latesty - 1) * LINE_DIST;
        QPoint center(centerx,centery);
        painter.drawEllipse(center,DOT_DIAM/2,DOT_DIAM/2);
    }

    //更新显示步数的label
    labelSteps->setText(QString::asprintf("    当前步数:%d",stepCount));

    //显示禁手
    QPen pen;
    pen.setColor(Qt::blue);
    pen.setWidth(3);
    painter.setPen(pen);
    if(ui->actionShowLosingPoints->isChecked() && game->getState() == Playing){
        for(int i = 1;i <= LINE_NUM;i++){
            for(int j = 1;j <= LINE_NUM;j++){
                if(game->getBoard(i,j) != 0) continue;
                if(game->isAbleToPlaceChess(i,j,game->getPlayer()) == false){
                    int centerx = MARGIN + (i - 1) * LINE_DIST;
                    int centery = MARGIN + (j - 1) * LINE_DIST;
                    QPoint center(centerx,centery);
                    painter.drawLine(centerx+5,centery+5,centerx-5,centery-5);
                    painter.drawLine(centerx+5,centery-5,centerx-5,centery+5);
                }
            }
        }
    }
    //设置悔棋按键是否可点击
    ui->actionRetract->setEnabled(stepCount != 0 && game->getState() == Playing);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(game->getState() == GameOver) return;
    //获取鼠标的坐标
    QPoint point = event->pos();
    int x = point.x();
    int y = point.y();
    //获取鼠标坐标横向和竖向相距最近的线的标号
    int lx = (x - MARGIN) / LINE_DIST + 1;
    int ly = (y - MARGIN) / LINE_DIST + 1;
    if(x - (MARGIN + (lx - 1) * LINE_DIST) > LINE_DIST / 2) lx++;
    if(y - (MARGIN + (ly - 1) * LINE_DIST) > LINE_DIST / 2) ly++;
    //获取邻近点的x,y坐标
    int px = MARGIN + (lx - 1) * LINE_DIST;
    int py = MARGIN + (ly - 1) * LINE_DIST;
    //判断该点是否合法
    if(lx > 0 && lx < LINE_NUM+1 && ly > 0 && ly < LINE_NUM+1 &&
            game->getBoard(lx,ly) == 0){
        //判断该点是否在合理距离内
        if((x-px)*(x-px) + (y-py)*(y-py) <= (SELECTED_DIAM/2)*(SELECTED_DIAM/2)){
            pointSelected = true;
            linex = lx;
            liney = ly;
        }
        else{
            pointSelected = false;
            linex = -1;
            liney = -1;
        }
    }
    else{
        pointSelected = false;
        linex = -1;
        liney = -1;
    }
    update(); //触发paintEvent事件
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    //在鼠标点下并松开后，再实现落子效果

    Q_UNUSED(event);
    if(game->getState() == GameOver) return;

    if(pointSelected == true){
        //将pointSelected重置为false,以防止鼠标未移动(即不会更新pointSelected)而在同一点连续落子
        pointSelected = false;

        if(game->getType() == PVE){
            if(canPlayMusic) sound->play(); //播放落子音效

            game->changeBoard(linex,liney,3-AIPlay);//修改board,已实现落子
            latestx = linex;
            latesty = liney;
            stepCount++;
            steps[stepCount][0] = latestx;
            steps[stepCount][1] = latesty;
            update();
            game->updateQi(linex,liney,3-AIPlay);
            game->checkWin(3-AIPlay);

            timer->stop();
            remainingTime = TIME_LIMIT;
            labelTimer->setText(QString::asprintf("    剩余时间:%d秒",remainingTime));

            if(game->getState() == GameOver) return;

            EvaluateValue ev = game->miniMax(-1000,1000,1,3,AIPlay);
            //qDebug() << '(' << ev.lx << ',' << ev.ly << ')' << "   " << ev.value;
            if(ev.lx == -1){
                //如果AI分析后传出来的点为(-1,-1),说明AI已无路可走，即玩家获胜
                game->onWin(AIPlay);
                return;
            }
            game->changeBoard(ev.lx,ev.ly,AIPlay);
            latestx = ev.lx;
            latesty = ev.ly;
            stepCount++;
            steps[stepCount][0] = latestx;
            steps[stepCount][1] = latesty;
            update();
            game->updateQi(ev.lx,ev.ly,AIPlay);
            game->checkWin(AIPlay);

            timer->start();

            isSaved = false;
        }
        else if(game->getType() == PVP){
            if(canPlayMusic) sound->play(); //播放落子音效

            game->changeBoard(linex,liney,game->getPlayer());//修改board,已实现落子
            latestx = linex;
            latesty = liney;
            stepCount++;
            steps[stepCount][0] = latestx;
            steps[stepCount][1] = latesty;
            update();
            game->updateQi(linex,liney,game->getPlayer());
            game->checkWin(game->getPlayer());

            remainingTime = TIME_LIMIT;
            labelTimer->setText(QString::asprintf("    剩余时间:%d秒",remainingTime));

            //交换player
            if(game->getPlayer() == Black) game->changePlayer(White);
            else game->changePlayer(Black);

            isSaved = false;
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //关闭窗口时，若当前棋局未结束且未保存，将弹出提示窗口
    timer->stop();

    if(game->getState() == Playing && isSaved == false){
        QMessageBox msgBox;
        msgBox.setWindowTitle("提示");
        msgBox.setInformativeText("当前棋局未结束且未保存，确认退出？");
        msgBox.setStandardButtons(QMessageBox::Cancel|QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Cancel,"返回");
        msgBox.setButtonText(QMessageBox::Ok,"仍然退出");
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Ok){
            event->accept();
        }else{
            event->ignore();
            timer->start();
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //当按下方向键，实现选中效果，移动提示点
    if(game->getState() == GameOver) return;

    if(event->key() == Qt::Key_Up or event->key() == Qt::Key_Down
            or event->key() == Qt::Key_Left or event->key() == Qt::Key_Right){
        pointSelected = true;
        if(linex == -1){
            linex = liney = (LINE_NUM+1)/2;
        }
        switch (event->key()) {
        case(Qt::Key_Up):
            liney--;
            break;
        case(Qt::Key_Down):
            liney++;
            break;
        case(Qt::Key_Left):
            linex--;
            break;
        case(Qt::Key_Right):
            linex++;
            break;
        }
        if(linex < 1 or linex > LINE_NUM or liney < 1 or liney > LINE_NUM){
            pointSelected = false;
        }
        if(game->getBoard(linex,liney) != 0){
            pointSelected = false;
        }
    }
    //当按下回车键，实现落子效果
    if(event->key() == Qt::Key_Return){
        mouseReleaseEvent(nullptr);
    }
    update();
}



void MainWindow::on_actionPVP_triggered()
{
    delete game;
    game = new GameModel;
    game->changeGameType(PVP);
    labelGameType->setText("当前模式：PVP");
    latestx = latesty = -1;
    stepCount = 0;
    remainingTime = TIME_LIMIT;
    labelTimer->setText(QString::asprintf("    剩余时间:%d秒",remainingTime));
    timer->start();
    isSaved = true;
    update();
}


void MainWindow::on_actionPVE_triggered()
{
    delete game;
    game = new GameModel;
    game->changeGameType(PVE);
    labelGameType->setText("当前模式：PVE");
    if(randomChooseForAIPlay){
        srand((unsigned)time(NULL));
        int random_number = rand();
        if(random_number % 2 == 0) AIPlay = Black;
        else AIPlay = White;
    }
    if(AIPlay == White){
        latestx = latesty = -1;
        stepCount = 0;

        game->changePlayer(Black);
    }else{
        //模拟第一步黑棋下在（1，1）点
        game->changeBoard(1,1,Black);
        latestx = 1;
        latesty = 1;
        stepCount = 1;
        steps[stepCount][0] = latestx;
        steps[stepCount][1] = latesty;
        update();
        game->updateQi(1,1,Black);

        game->changePlayer(White);
    }

    remainingTime = TIME_LIMIT;
    labelTimer->setText(QString::asprintf("    剩余时间:%d秒",remainingTime));
    timer->start();
    isSaved = true;
    update();
}


void MainWindow::on_actionSave_triggered()
{
    //存档
    timer->stop();

    QString dlgTitle = "选择一个文件";
    QString curPath = QCoreApplication::applicationDirPath();
    QString filter = "文本文件(*.txt)";
    QString fileName = QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);
    if(fileName.isEmpty()){
        timer->start();
        return;
    }
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QString str = "";
        //记录棋盘大小
        str += (LINE_NUM / 10) + '0';
        str += (LINE_NUM % 10) + '0';

        switch(game->getType()){
        case(PVP):
            str += "10";
            break;
        case(PVE):
            str += "2";
            if(AIPlay == Black) str += '1';
            else if(AIPlay == White) str += '2';
            break;
        }

        switch(game->getState()){
        case(Playing):
            str += "1";
            break;
        case(GameOver):
            str += "2";
            break;
        case(Suspension):
            str += "3";
            break;
        }

        switch(game->getPlayer()){
        case(Black):
            str += "1";
            break;
        case(White):
            str += "2";
            break;
        }

        for(int i = 0;i < LINE_NUM+2;i++){
            for(int j = 0;j < LINE_NUM+2;j++){
                str += game->getBoard(i,j)+'0';
            }
        }
        QByteArray strBytes = str.toUtf8();
        file.write(strBytes,strBytes.length());
        file.close();

        isSaved = true;
    }

    timer->start();
}

void MainWindow::on_actionrestore_last_saved_board_triggered()
{
    //读档
    timer->stop();

    QString dlgTitle = "打开一个文件";
    QString curPath = QCoreApplication::applicationDirPath();
    QString filter = "文本文件(*.txt)";
    QString fileName = QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
    if(fileName.isEmpty()){
        timer->start();
        return;
    }
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
        string str = file.readAll().toStdString();

        int boardSize = (str[0] - '0') * 10 + (str[1] - '0');
        LINE_NUM = boardSize;
        W = MARGIN * 2 + (LINE_NUM - 1) * LINE_DIST;
        H = MARGIN * 2 + (LINE_NUM - 1) * LINE_DIST;
        setFixedSize(W,H);

        if(str[2] == '1'){
            delete game;
            game = new GameModel;
            game->changeGameType(PVP);
            labelGameType->setText("当前模式：PVP");
            latestx = latesty = -1;
            stepCount = 0;
        }else if(str[2] == '2'){
            delete game;
            game = new GameModel;
            game->changeGameType(PVE);
            labelGameType->setText("当前模式：PVE");
            latestx = latesty = -1;
            stepCount = 0;

            if(str[3] == '1') AIPlay = Black;
            else if(str[3] == '2') AIPlay = White;
        }

        if(str[4] == '1') game->changeGameState(Playing);
        else if(str[4] == '2') game->changeGameState(GameOver);
        else if(str[4] == '3') game->changeGameState(Suspension);
        file.close();

        if(str[5] == '1') game->changePlayer(Black);
        else if(str[5] == '2') game->changePlayer(White);

        int idx = 6;
        for(int i = 0;i < LINE_NUM+2;i++){
            for(int j = 0;j < LINE_NUM+2;j++){
                if(str[idx] == '0') game->changeBoard(i,j,0);
                if(str[idx] == '1') {
                    game->changeBoard(i,j,1);
                    stepCount++;
                    steps[stepCount][0] = i;
                    steps[stepCount][1] = j;
                    game->updateQi(i,j,1);
                }
                if(str[idx] == '2') {
                    game->changeBoard(i,j,2);
                    stepCount++;
                    steps[stepCount][0] = i;
                    steps[stepCount][1] = j;
                    game->updateQi(i,j,2);
                }
                if(str[idx] == '3') game->changeBoard(i,j,3);
                idx++;
            }
        }
        update();
        file.close();

        isSaved = true;
    }

    remainingTime = TIME_LIMIT;
    labelTimer->setText(QString::asprintf("    剩余时间:%d秒",remainingTime));
    timer->start();
}

void MainWindow::on_actionShowGameRules_triggered()
{
    //链接到外部网站，获取不围棋规则
    timer->stop();
    QDesktopServices::openUrl(QUrl("https://baike.baidu.com/item/%E4%B8%8D%E5%9B%B4%E6%A3%8B/19604744?fr=aladdin"));
}

void MainWindow::on_timeOut()
{
    if(game->getState() != Playing){
        timer->stop();
        return;
    }
    remainingTime--;
    labelTimer->setText(QString::asprintf("    剩余时间:%d秒",remainingTime));
    if(remainingTime == 0){
        timer->stop();
        timeLimitExceeded();
    }
}

void MainWindow::timeLimitExceeded()
{
    QString winText[2] = {"白棋超时，黑棋获胜","黑棋超时，白棋获胜"};
    game->changeGameState(GameOver);
    QMessageBox::information(nullptr,"游戏结束",winText[2-game->getPlayer()]);
}


void MainWindow::on_actionRetract_triggered()
{
    if(game->getState() != Playing) return;
    if(stepCount == 0) return;
    if(game->getType() == PVP){
        stepCount--;
        latestx = steps[stepCount][0];
        latesty = steps[stepCount][1];
        delete game;
        game = new GameModel;
        game->changeGameType(PVP);
        if(stepCount % 2 == 0) game->changePlayer(Black);
        else game->changePlayer(White);
        for(int i = 1;i <= stepCount;i++){
            int x = steps[i][0];
            int y = steps[i][1];
            game->changeBoard(x,y,2-i%2);
            game->updateQi(x,y,2-i%2);
        }
        remainingTime = TIME_LIMIT;
        update();
    }
    else if(game->getType() == PVE){
        stepCount -= 2;
        latestx = steps[stepCount][0];
        latesty = steps[stepCount][1];
        delete game;
        game = new GameModel;
        game->changeGameType(PVE);
        if(AIPlay == White) game->changePlayer(Black);
        else game->changePlayer(White);
        for(int i = 1;i <= stepCount;i++){
            int x = steps[i][0];
            int y = steps[i][1];
            game->changeBoard(x,y,2-i%2);
            game->updateQi(x,y,2-i%2);
        }
        remainingTime = TIME_LIMIT;
        update();
    }
}



void MainWindow::on_actionShowLosingPoints_triggered(bool checked)
{
    Q_UNUSED(checked);
    update();
}


void MainWindow::on_actionShowGameInfo_triggered()
{
    timer->stop();
    QString str = "欢迎来到NoGo游戏！以下是关于游戏部分操作的说明。\n"
                  "\n"
                  "新游戏：\n"
                  "    1、PVP模式：黑白棋交替落子，均由玩家通过鼠标点击或键盘操作实现\n"
                  "    2、PVE模式：玩家鼠标点击或键盘操作落子，AI分析后自动落子\n"
                  "存/读档：\n"
                  "    1、保存当前状态：在当前目录下选择或创建一个txt文件，将当前棋盘数据保存入该文档中\n"
                  "    2、恢复上次状态：在当前目录下选择一个txt文件，程序会读入文件中的数据，并显示到棋盘上\n"
                  "技能：\n"
                  "    1、显示禁手：选中该项后，每次轮到某方落子时，棋盘上会显示出该方不能下的点（用叉表示）\n"
                  "    2、悔棋：PVP模式下，退回一步；PVE模式下，退回两步，即黑白棋各退回一步\n"
                  "帮助：\n"
                  "    1、不围棋规则：点击后，利用浏览器跳转至百度百科关于不围棋的词条\n"
                  "    2、操作说明：你可以随时通过点击此项来呼出本窗口\n"
                  "自定义设置：\n"
                  "    玩家可自定义棋盘大小（7*7-15*15，默认9*9）、每步棋限时（5-60秒，默认30）、是否开启"
                  "提示音以及调节音量、修改AI执子颜色\n"
                  "\n"
                  "游戏时间限制："
                  "    每步子有限制时间，默认为30秒，超过限制时间自动判负\n"
                  "默认开始游戏："
                  "    进入游戏后，默认为PVP模式，且计时立刻开始\n"
                  "键盘操作落子方式："
                  "    利用键盘方向键移动选中点，按回车键落子\n"
            ;


    QMessageBox msgBox;
    msgBox.setWindowTitle("操作说明");
    msgBox.setInformativeText(str);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
    timer->start();
}


void MainWindow::on_actionSettings_triggered()
{
    timer->stop();

    DialogSettings * dlg = new DialogSettings();
    dlg->setDefaultValue(canPlayMusic,player->volume(),randomChooseForAIPlay,AIPlay);
    int ret = dlg->exec();
    if(ret == QDialog::Accepted){
        int timeLimit = dlg->getTimeLimit();
        int boardSize = dlg->getBoardSize();
        bool chessMusic = dlg->getChessMusic();
        int bgmVolume = dlg->getBGMVolume();
        int AIPlayMode = dlg->getAIPlayMode();

        TIME_LIMIT = timeLimit;
        canPlayMusic = chessMusic;
        player->setVolume(bgmVolume);
        if(AIPlayMode == 2) randomChooseForAIPlay = true;
        else{
            randomChooseForAIPlay = false;
            if(AIPlayMode == 0) AIPlay = Black;
            else if(AIPlayMode == 1) AIPlay = White;
        }

        if(boardSize != LINE_NUM){
            if(game->getState() == Playing && isSaved == false){
                QMessageBox msgBox;
                msgBox.setWindowTitle("提示");
                msgBox.setInformativeText("修改棋盘大小会重新创建棋局，而当前棋局未结束且未保存，确认修改？");
                msgBox.setStandardButtons(QMessageBox::Cancel|QMessageBox::Ok);
                msgBox.setButtonText(QMessageBox::Cancel,"取消修改");
                msgBox.setButtonText(QMessageBox::Ok,"仍然修改");
                msgBox.setDefaultButton(QMessageBox::Ok);
                int ret = msgBox.exec();
                if(ret != QMessageBox::Ok){
                    timer->start();
                    return;
                }
            }

            LINE_NUM = boardSize;
            W = MARGIN * 2 + (LINE_NUM - 1) * LINE_DIST;
            H = MARGIN * 2 + (LINE_NUM - 1) * LINE_DIST;
            setFixedSize(W,H); //为窗口设置固定大小
            if(game->getType() == PVP){
                on_actionPVP_triggered();
            }else{
                on_actionPVE_triggered();
            }
        }
    }

    timer->start();
}
