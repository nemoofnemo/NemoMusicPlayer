#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_NemoMusicPlayer.h"
#include "player_utilities.h"
#include "libnemo/nemo_utilities.h"

class NemoMusicPlayer : public QMainWindow
{
    Q_OBJECT

public:
    NemoMusicPlayer(QWidget *parent = Q_NULLPTR);

private:
    Ui::NemoMusicPlayerClass ui;
};
