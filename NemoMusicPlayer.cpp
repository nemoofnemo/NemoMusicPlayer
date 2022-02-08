#include "NemoMusicPlayer.h"
#include <fstream>

NemoMusicPlayer::NemoMusicPlayer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.testButton, &QPushButton::clicked, this, &NemoMusicPlayer::onTest);
}

void NemoMusicPlayer::onTest(bool checked) {
    nDebug("onTest");
    debug_test();
}

NemoMusicPlayer::~NemoMusicPlayer()
{
}

void NemoMusicPlayer::debug_test(void)
{
    //./ffplay -f f32le -ac 2 -ar 48000 "D:\mov\xxxx"

    nemo::ByteArray arr;
    loader.open(std::string("D:\\mov\\test2.mov"));
    //loader.seek(std::chrono::milliseconds(2000));
    loader.load(&arr);
    loader.close();

   /* std::ofstream out;
    out.open("D:\\mov\\xxxx",
        std::ofstream::trunc | std::ofstream::binary | std::ofstream::out);
    out.write((const char*)arr.get(), arr.size());
    out.close();*/
}
