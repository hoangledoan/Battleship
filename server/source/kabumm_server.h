#pragma once

#include "connection.h"

#include <memory>

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QHostAddress>
#include <QDataStream>
#include <QTimer>

namespace kbm {
    class KabummServer : public QObject {
        Q_OBJECT

        std::unique_ptr<QTcpServer> server = std::make_unique<QTcpServer>();
        std::unique_ptr<QTimer> timer = std::make_unique<QTimer>();
        std::vector<std::shared_ptr<Connection>> connections = std::vector<std::shared_ptr<Connection>>();

        void handle_event();

    public:
        KabummServer();
        ~KabummServer() = default;
    };
}
