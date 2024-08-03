#include "kabumm_server.h"
#include "connection.h"
#include "message/message.h"

#include <iostream>

namespace kbm {
    void KabummServer::handle_event() {
        std::cout << "connections: " << connections.size() << std::endl;

        while (server->hasPendingConnections()) {
            QTcpSocket* socket = server->nextPendingConnection();

            connect(socket, &QTcpSocket::aboutToClose, this, &KabummServer::handle_event);

            connections.emplace_back(std::make_shared<Connection>(socket));
            connections[connections.size() - 1]->init();
        }

        for (uint32_t i = 0; i < connections.size(); i++) {
            Connection& connection = *connections[i];
            time_t now = std::time(nullptr);
            int64_t available_bytes = connection.get_socket().bytesAvailable();

            if (!connection.get_socket().isOpen() || now - connection.get_last_seen() > 10) {
                connection.set_closing(true);
                continue;
            }

            if (available_bytes > 0) {
                connection.seen_now();

                std::vector<char> buf = std::vector<char>(available_bytes);
                connection.get_socket().read(buf.data(), available_bytes);

                Message<Unsafe> raw_msg = Message<Unsafe>(std::move(buf));
                auto parsed_msg = raw_msg.parse();
                for (auto& msg : parsed_msg) {
                    std::visit([&connection](auto& msg){
                        if constexpr (!std::convertible_to<Message<Safe<UnknownMsg>>, decltype(msg)>) {
                            connection.process_message(msg);
                        }
                    }, msg);
                }
            }

            connection.timer();
        }

        std::erase_if(connections, [](auto& e){
            return e->is_closing();
        });

        std::optional<uint32_t> partner;
        for (uint32_t i = 0; i < connections.size(); i++) {
            Connection& con = *connections[i];
            if (con.is_waiting()) {
                if (partner) {
                    Connection& partner_con = *connections[partner.value()];

                    if (!con.is_compatible() && !partner_con.is_compatible()) {
                        continue;
                    } else if (!con.is_compatible()) {
                        connections[i]->connect(connections[partner.value()], false);
                        connections[partner.value()]->connect(connections[i], true);
                    } else {
                        connections[i]->connect(connections[partner.value()], true);
                        connections[partner.value()]->connect(connections[i], false);
                    }
                } else {
                    partner = i;
                }
            }
        }
    }

    KabummServer::KabummServer() {
        connect(server.get(), &QTcpServer::newConnection, this, &KabummServer::handle_event);
        server->listen(QHostAddress::Any, 8080);

        connect(timer.get(), &QTimer::timeout, this, &KabummServer::handle_event);
        timer->start(2000);
    }
}