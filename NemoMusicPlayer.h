#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_NemoMusicPlayer.h"

class NemoMusicPlayer : public QMainWindow
{
    Q_OBJECT

public:
    NemoMusicPlayer(QWidget *parent = Q_NULLPTR);

private:
    Ui::NemoMusicPlayerClass ui;
};
