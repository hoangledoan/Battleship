#include "network.h"
#include "message.h"
#include "kinds.h"

#include <iostream>

namespace kbm::net {
    void Network::server_connect() {
        if (!socket && state->network_state == State::NetworkState::WaitingForConnection) {
            socket = std::unique_ptr<QTcpSocket>(server->nextPendingConnection());
            stream->setDevice(socket.value().get());
            connect(socket.value().get(), &QTcpSocket::readyRead, this, &Network::handle_message);
            *stream << IdMsg(CLIENT_ID);
            *stream << ParamMsg(
                    state->width,
                    state->height,
                    state->n_battleships,
                    state->n_cruisers,
                    state->n_destroyers,
                    state->n_submarines
            );
            state->our_turn = true;
            state->network_state = State::NetworkState::WaitingForBegin;
        }
    }

    void Network::socket_connect() {
        if (state->network_state == State::NetworkState::WaitingForConnection) {
            stream->setDevice(socket.value().get());
            connect(socket.value().get(), &QTcpSocket::readyRead, this, &Network::handle_message);
            *stream << IdMsg(CLIENT_ID);
            state->our_turn = false;
            state->network_state = State::NetworkState::WaitingForParams;
        }
    }

    void Network::handle_message() {
        int64_t available_bytes = socket.value()->bytesAvailable();
        available_bytes = std::min(available_bytes, 256ll); // An honest client should never send messages this long
        std::vector<char> buf = std::vector<char>(available_bytes);
        int64_t read_bytes = socket.value()->read(buf.data(), available_bytes);
        buf.resize(read_bytes);
        std::vector<std::unique_ptr<kbm::net::Message>> msgs = Message::in(buf, state->compatible);

        for (auto &msg: msgs) {
            switch (state->network_state) {
                case State::NetworkState::WaitingForParams: {
                    if (auto param_msg = dynamic_cast<ParamMsg *>(msg.get()); param_msg) {
                        // State: Waiting for game parameters and received parameters message
                        state->width = param_msg->width;
                        state->height = param_msg->width;
                        state->n_battleships = param_msg->n_battleships;
                        state->n_cruisers = param_msg->n_cruisers;
                        state->n_destroyers = param_msg->n_destroyers;
                        state->n_submarines = param_msg->n_submarines;
                        game->set_field(state->width, state->height);
                        state->received_game_parameters = true;
                        state->network_state = State::NetworkState::WaitingForBegin;
                    } else if (auto id_msg = dynamic_cast<IdMsg *>(msg.get()); id_msg) {
                        // State: Waiting for game parameters and received id message
                        if (id_msg->id == CLIENT_ID) {
                            state->compatible = true;
                            *stream << ConnectMsg(state->our_name);
                        }
                    } else if (auto connect_msg = dynamic_cast<ConnectMsg *>(msg.get()); connect_msg) {
                        // State: Waiting for game parameters and received connect message
                        state->their_name = connect_msg->name;
                        state->received_name = true;
                    }
                    break;
                }
                case State::NetworkState::WaitingForBegin: {
                    if (auto begin_msg = dynamic_cast<BeginMsg *>(msg.get()); begin_msg) {
                        // State: Waiting for game begin and received begin message
                        if (state->client_mode == State::ClientMode::Client) {
                            if (state->ready_to_begin) {
                                state->ready_to_begin = false;
                                state->network_state = State::NetworkState::InGame;
                                state->received_begin_confirmation = true;
                                *stream << StatusMsg(StatusMsg::StatusCode::Ok);
                            } else {
                                *stream << StatusMsg(StatusMsg::StatusCode::NotReady);
                            }
                        }
                    } else if (auto status_msg = dynamic_cast<StatusMsg *>(msg.get()); status_msg) {
                        // State: Waiting for game begin and received response to begin message
                        if (state->client_mode == State::ClientMode::Server) {
                            if (status_msg->code == StatusMsg::StatusCode::Ok) {
                                state->ready_to_begin = false;
                                state->network_state = State::NetworkState::InGame;
                                state->received_begin_confirmation = true;
                            }
                        }
                    } else if (auto id_msg = dynamic_cast<IdMsg *>(msg.get()); id_msg) {
                        // State: Waiting for game begin and received id message
                        if (id_msg->id == CLIENT_ID) {
                            state->compatible = true;
                            *stream << ConnectMsg(state->our_name);
                        }
                    } else if (auto connect_msg = dynamic_cast<ConnectMsg *>(msg.get()); connect_msg) {
                        // State: Waiting for game parameters and received connect message
                        state->their_name = connect_msg->name;
                        state->received_name = true;
                    }
                    break;
                }
                case State::NetworkState::InGame: {
                    if (auto shot_msg = dynamic_cast<ShotMsg *>(msg.get()); shot_msg) {
                        // State: In game and received shot
                        if (!state->our_turn) {
                            state->received_shot = true;
                            auto [status, kind, hit_pos] = game->get_status(shot_msg->x, shot_msg->y);
                            if (status == "Missed") {
                                state->shot_self = State::ShotResponse::Missed;
                                state->our_turn = true;
                                *stream << ResultMsg(ResultMsg::ResultCode::Missed);
                            } else if (status == "Hit") {
                                state->shot_self = State::ShotResponse::Hit;
                                state->our_turn = true;
                                *stream << ResultMsg(ResultMsg::ResultCode::Hit);
                            } else if (status == "Sunk") {
                                state->shot_self = State::ShotResponse::Sunk;
                                state->our_turn = true;
                                *stream << ResultMsg(ResultMsg::ResultCode::HitSunk, hit_pos);
                            } else if (status == "Lost") {
                                state->shot_self = State::ShotResponse::Won;
                                state->disconnect = true;
                                *stream << ResultMsg(ResultMsg::ResultCode::HitWon, hit_pos);
                            } else if (status == "Invalid") {
                                *stream << ResultMsg(ResultMsg::ResultCode::OutsideField);
                            }
                        } else {
                            *stream << ResultMsg(ResultMsg::ResultCode::NotYourTurn);
                        }
                    } else if (auto result_msg = dynamic_cast<ResultMsg *>(msg.get()); result_msg) {
                        // State: In game and received shot response
                        state->received_shot_response = true;
                        switch (result_msg->code) {
                            case ResultMsg::Missed: {
                                state->shot_other = State::ShotResponse::Missed;
                                break;
                            }
                            case ResultMsg::Hit: {
                                state->shot_other = State::ShotResponse::Hit;
                                break;
                            }
                            case ResultMsg::HitSunk: {
                                state->shot_other = State::ShotResponse::Sunk;
                                state->sunk_positions.insert(state->sunk_positions.end(), result_msg->hits.begin(), result_msg->hits.end());
                                break;
                            }
                            case ResultMsg::HitWon: {
                                state->shot_other = State::ShotResponse::Won;
                                state->sunk_positions.insert(state->sunk_positions.end(), result_msg->hits.begin(), result_msg->hits.end());
                                state->disconnect = true;
                                break;
                            }
                            case ResultMsg::NotYourTurn: {
                                std::cout << "The other player may be cheating" << std::endl;
                                *stream << ShotMsg(state->shoot_x, state->shoot_y);
                                break;
                            }
                            case ResultMsg::OutsideField: {
                                std::cout << "The other player may be cheating" << std::endl;
                                *stream << ShotMsg(state->shoot_x, state->shoot_y);
                                break;
                            }
                            case ResultMsg::OtherError: {
                                std::cout << "The other player may be cheating" << std::endl;
                                *stream << ShotMsg(state->shoot_x, state->shoot_y);
                                break;
                            }
                        }
                    }
                    break;
                }
                default: {
                }
            }
        }

        emit update_ui();
    }

    void Network::open_server() {
        socket = {};
        connect(server.get(), &QTcpServer::newConnection, this, &Network::server_connect);
        if (server->isListening()) {
            server->close();
        }
        server->listen(QHostAddress::Any, state->port);
    }

    void Network::open_client() {
        socket = std::make_unique<QTcpSocket>();
        socket.value()->connectToHost(QString::fromStdString(state->host), state->port);
        connect(socket.value().get(), &QTcpSocket::connected, this, &Network::socket_connect);
    }

    void Network::handle_event() {
        if (state->start_client_or_server) {
            // State: Server or client (re)started
            state->start_client_or_server = false;
            state->compatible = false;
            state->network_state = State::NetworkState::WaitingForConnection;

            switch (state->client_mode) {
                case State::ClientMode::Server: {
                    open_server();
                    break;
                }
                case State::ClientMode::Client: {
                    open_client();
                    break;
                }
            }
        }

        if (state->timer_tick) {
            if (state->network_state == State::NetworkState::WaitingForBegin) {
                // State: Waiting for game begin and timer ticked
                if (state->client_mode == State::ClientMode::Server && state->ready_to_begin) {
                    *stream << BeginMsg();
                }
            }
        }

        if (state->shoot) {
            if (state->network_state == State::NetworkState::InGame) {
                // State: In game and UI signalled shot
                if (state->our_turn) {
                    state->shoot = false;
                    state->our_turn = false;
                    *stream << ShotMsg(state->shoot_x, state->shoot_y);
                }
            }
        }
    }

    Network::Network(std::shared_ptr<State> state, std::shared_ptr<Game> game) : state(std::move(state)), game(std::move(game)) {
        this->state->network_state = State::NetworkState::Unininitialized;
    }
}
