#include "connection.h"

namespace kbm {
    QTcpSocket& Connection::get_socket() {
        return *socket;
    }

    std::time_t Connection::get_last_seen() {
        return last_seen;
    }

    bool Connection::is_closing() {
        return closing;
    }

    bool Connection::is_waiting() {
        return state == ConnectionState::IdCompatible || state == ConnectionState::IdNotCompatible;
    }

    bool Connection::is_compatible() {
        return state == ConnectionState::IdCompatible;
    }

    void Connection::set_closing(bool c) {
        if (partner) {
            (*partner)->closing = c;
        }
        closing = c;
    }


    void Connection::seen_now() {
        last_seen = std::time(nullptr);
    }

    void Connection::init() {
        send_message(IdMsg(0x33));
    }

    void Connection::timer() {
        switch (state) {
            case ConnectionState::Uninitialized: {
                if (std::time(nullptr) - last_seen > 5) {
                    state = ConnectionState::IdNotCompatible;
                }
                break;
            }
            case ConnectionState::Ended: {
                set_closing(true);
            }
            case ConnectionState::WaitingNotCompatible: {
                send_message(BeginMsg());
            }
            case ConnectionState::ReadyCompatible: {
                if (partner.value()->state == ConnectionState::ReadyCompatible) {
                    state = ConnectionState::InGame;
                    partner.value()->state = ConnectionState::InGame;
                    send_message(StatusMsg(0x00));
                    partner.value()->send_message(StatusMsg(0x00));
                } else if (partner.value()->state == ConnectionState::ReadyNotCompatible) {
                    state = ConnectionState::InGame;
                    partner.value()->state = ConnectionState::InGame;
                    send_message(StatusMsg(0x00));
                }
            }
            case ConnectionState::ReadyNotCompatible: {
                if (partner.value()->state == ConnectionState::ReadyCompatible) {
                    state = ConnectionState::InGame;
                    partner.value()->state = ConnectionState::InGame;
                    partner.value()->send_message(StatusMsg(0x00));
                }
            }
            default: {}
        }
    }

    void Connection::connect(const std::shared_ptr<Connection>& p, bool first) {
        partner = p;

        switch (state) {
            case ConnectionState::IdCompatible: {
                send_message(ConfirmMsg(first, partner.value()->name));
                send_message(ParamMsg(10, 10, 1, 2, 3, 4));
                state = ConnectionState::WaitingCompatible;
                break;
            }
            case ConnectionState::IdNotCompatible: {
                send_message(ParamMsg(10, 10, 1, 2, 3, 4));
                state = ConnectionState::WaitingNotCompatible;
                break;
            }
            default: {
                assert(false);
            }
        }
    }

    Connection::Connection(QTcpSocket *socket) : socket(socket) {
        seen_now();
    }
}