#include "HFsLib.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);                 // главное приложение
    HFsLib w;                                   // обьект главного виджета
    w.setWindowIcon(QIcon("wing.png"));         // установление картинки в угол окна
    w.show();                                   // вывод главного виджета
    return a.exec();                            // возвратить результат вывполнения работы окна
}