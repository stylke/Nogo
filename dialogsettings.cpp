#include "dialogsettings.h"
#include "ui_dialogsettings.h"
#include "gamemodel.h"

extern int TIME_LIMIT;
extern int LINE_NUM;

DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);
}

DialogSettings::~DialogSettings()
{
    delete ui;
}

void DialogSettings::setDefaultValue(bool chessMusic,int bgmVolumn,bool randomChooseForAIPlay,int AIPlay)
{
    ui->spinBoxTimeLimit->setValue(TIME_LIMIT);
    ui->spinBoxBoardSize->setValue(LINE_NUM);
    ui->checkBox->setChecked(chessMusic);
    ui->horizontalSlider->setValue(bgmVolumn);
    int AIPlayMode;
    if(randomChooseForAIPlay == true) AIPlayMode = 2;
    else AIPlayMode = AIPlay - 1;
    ui->comboBox->setCurrentIndex(AIPlayMode);
}

int DialogSettings::getTimeLimit()
{
    return ui->spinBoxTimeLimit->value();
}

int DialogSettings::getBoardSize()
{
    return ui->spinBoxBoardSize->value();
}

bool DialogSettings::getChessMusic()
{
    return ui->checkBox->isChecked();
}

int DialogSettings::getBGMVolume()
{
    return ui->horizontalSlider->value();
}

int DialogSettings::getAIPlayMode()
{
    return ui->comboBox->currentIndex();
}


void DialogSettings::on_horizontalSlider_valueChanged(int value)
{
    ui->labelBGMVolumn->setText(QString::asprintf("%d%%",value));
}
