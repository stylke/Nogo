#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>


namespace Ui {
class DialogSettings;
}

class DialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(QWidget *parent = nullptr);
    ~DialogSettings();

    void setDefaultValue(bool chessMusic,int bgmVolumn,bool randomChooseForAIPlay,int AIPlay);
    int getTimeLimit();
    int getBoardSize();
    bool getChessMusic();
    int getBGMVolume();
    int getAIPlayMode();

private slots:
    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::DialogSettings *ui;
};

#endif // DIALOGSETTINGS_H
