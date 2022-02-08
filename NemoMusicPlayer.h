#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_NemoMusicPlayer.h"
#include "player_utilities.h"
#include "libnemo/nemo_utilities.h"
#include "NemoAudioLoader.h"

class NemoMusicPlayer : public QMainWindow
{
    Q_OBJECT

public:
    NemoMusicPlayer(QWidget *parent = Q_NULLPTR);
    virtual ~NemoMusicPlayer();

public slots:
    void onTest(bool checked);

private:
    Ui::NemoMusicPlayerClass ui;
    NemoAudioLoader loader;
};
