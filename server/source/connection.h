#pragma once

#include <memory>

#include <QTcpSocket>

#include "message/kinds.h"
#include "message/message.h"

namespace kbm {
    enum class ConnectionState {
        Uninitialized,
        Connecting,
        IdNotCompatible,
        IdCompatible,
        WaitingNotCompatible,
        WaitingCompatible,
        ReadyNotCompatible,
        ReadyCompatible,
        InGame,
        Ended
    };

    class Connection;

    class Connection {
        std::unique_ptr<QTcpSocket> socket;
        std::time_t last_seen;
        ConnectionState state = ConnectionState::Uninitialized;
        std::string name = "No Name";
        std::optional<std::shared_ptr<Connection>> partner;
        bool closing;

    public:
        QTcpSocket& get_socket();
        std::time_t get_last_seen();
        bool is_closing();
        bool is_waiting();
        bool is_compatible();

        void set_closing(bool c);

        void seen_now();

        void init();

        template<typename MSG>
        void process_message(MSG msg) {
            std::cout << typeid(msg).name() << std::endl;
            switch (state) {
                case ConnectionState::Uninitialized: {
                    if constexpr (std::convertible_to<MSG, Message<Safe<IdMsg>>>) {
                        if ((*msg).id == 0x03) {
                            state = ConnectionState::Connecting;
                        } else {
                            state = ConnectionState::IdNotCompatible;
                        }
                        send_message(StatusMsg(0x00));
                    }
                    break;
                }
                case ConnectionState::Connecting: {
                    if constexpr (std::convertible_to<MSG, Message<Safe<ConnectMsg>>>) {
                        name = (*msg).name;
                        state = ConnectionState::IdCompatible;
                        send_message(StatusMsg(0x00));
                    }
                    break;
                }
                case ConnectionState::WaitingNotCompatible: {
                    if constexpr (std::convertible_to<MSG, Message<Safe<StatusMsg>>>) {
                        if ((*msg).code == 0x00) {
                            state = ConnectionState::ReadyNotCompatible;
                        }
                    }
                    break;
                }
                case ConnectionState::WaitingCompatible: {
                    if constexpr (std::convertible_to<MSG, Message<Safe<BeginMsg>>>) {
                        state = ConnectionState::ReadyCompatible;
                    }
                    break;
                }
                case ConnectionState::InGame: {
                    partner.value()->send_message(msg);
                    break;
                }
                default: {}
            }
        }

        void timer();

        void connect(const std::shared_ptr<Connection>& p, bool first);

        template<typename MSG>
        void send_message(MSG msg) {
            auto buf = std::vector<char>();

            if constexpr (std::convertible_to<MSG, ParamMsg>) {
                buf.emplace_back(ParamMsg::cmd);
                buf.emplace_back(msg.width);
                buf.emplace_back(msg.height);
                buf.emplace_back(msg.n_battleships);
                buf.emplace_back(msg.n_cruisers);
                buf.emplace_back(msg.n_destroyers);
                buf.emplace_back(msg.n_submarines);
            } else if constexpr (std::convertible_to<MSG, BeginMsg>) {
                buf.emplace_back(BeginMsg::cmd);
            } else if constexpr (std::convertible_to<MSG, ShotMsg>) {
                buf.emplace_back(ShotMsg::cmd);
            } else if constexpr (std::convertible_to<MSG, StatusMsg>) {
                buf.emplace_back(StatusMsg::cmd);
                buf.emplace_back(msg.code);
            } else if constexpr (std::convertible_to<MSG, ResultMsg>) {
                buf.emplace_back(ResultMsg::cmd);
                buf.emplace_back(msg.code);
            } else if constexpr (std::convertible_to<MSG, IdMsg>) {
                buf.emplace_back(IdMsg::cmd);
                buf.emplace_back(msg.id);
            } else if constexpr (std::convertible_to<MSG, ConnectMsg>) {
                buf.emplace_back(ConnectMsg::cmd);
            } else if constexpr (std::convertible_to<MSG, ConfirmMsg>) {
                buf.emplace_back(ConfirmMsg::cmd);
            }

            socket->write(buf.data(), buf.size());
        }

        explicit Connection(QTcpSocket* socket);
        ~Connection() = default;
    };
}
