#include "net/network.h"
#include "kabumm.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/Img_file/app.ico"));

    Kabumm w;
    w.main_ui->setWindowTitle("Kabumm");
    w.main_ui->show();

    return app.exec();
}
