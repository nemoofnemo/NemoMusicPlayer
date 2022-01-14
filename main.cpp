#include "NemoMusicPlayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NemoMusicPlayer w;
    w.show();
    return a.exec();
}
