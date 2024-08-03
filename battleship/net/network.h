#pragma once

#include "state.h"
#include "logic/game.hpp"

#include <memory>

#include <QDataStream>
#include <QTcpServer>
#include <QTcpSocket>
#include <utility>

namespace kbm::net {
    constexpr uint8_t CLIENT_ID = 0x03; /**< @brief IdMsg group ID for the Kabumm client/server */
    constexpr uint8_t SERVER_ID = 0x33; /**< @brief IdMsg group ID for the Kabumm matchmaking server */

    /**
     * @brief Processor for all Network-related events
     */
    class Network : public QObject {
    Q_OBJECT

        std::unique_ptr<QTcpServer> server = std::make_unique<QTcpServer>();
        std::optional<std::unique_ptr<QTcpSocket>> socket = {};
        std::unique_ptr<QDataStream> stream = std::make_unique<QDataStream>();

        std::shared_ptr<State> state;
        std::shared_ptr<Game> game;

        void server_connect();

        void socket_connect();

        void handle_message();

        void open_server();

        void open_client();

    public:
        /**
         * @brief Delete the copy constructor
         */
        Network(const Network &) = delete;

        /**
         * @brief Construct a Network with a shared State and Game
         * @param state General State shared between UI and Network
         * @param game Game state shared between UI and Network
         */
        Network(std::shared_ptr<State> state, std::shared_ptr<Game> game);

    public slots:
        /**
         * @brief Process UI and background events (event details are passed through the shared State object)
         */
        void handle_event();

    signals:
        /**
         * @brief Update the UI based on Network events (event details are passed through the shared State object)
         */
        void update_ui();
    };
}
