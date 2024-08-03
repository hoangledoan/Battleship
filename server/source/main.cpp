#include <QCoreApplication>

#include "kabumm_server.h"

int main(int argc, char* argv[]) {
    QCoreApplication app = QCoreApplication(argc, argv);
    kbm::KabummServer server = kbm::KabummServer();
    return QCoreApplication::exec();
}
