#include "CryptDlg/GostCrypt.h"
#include <QApplication>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    GostCrypt w;
    w.show();

    return a.exec();
}
