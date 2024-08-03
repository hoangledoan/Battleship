#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <QHostAddress>

namespace kbm {
    /**
     * @brief Holder of state between the UI and Network not directly related to the game itself.
     *
     * The State struct serves as a container for various state information used in communication
     * between the UI and the network module in the Kabumm game application. It includes network
     * parameters, game parameters, and various flags and data related to game events.
     */
    struct State {
        /**
         * @brief Print a debug representation of the State.
         *
         * This function prints a debug representation of the State struct to the standard output.
         * It includes the values of various flags, network state, network parameters, and game parameters.
         */
        void debug_print() const;

        // Type definitions
        /**
         * @brief Enumerates the client modes.
         */
        enum class ClientMode {
            Server, ///< The application is in server mode.
            Client  ///< The application is in client mode.
        };

        /**
         * @brief Enumerates the network states.
         */
        enum class NetworkState {
            Unininitialized,      ///< The network is uninitialized.
            WaitingForConnection, ///< Waiting for a network connection.
            WaitingForParams,     ///< Waiting for network parameters.
            WaitingForBegin,      ///< Waiting for the game to begin.
            InGame                ///< The game is in progress.
        };

        /**
         * @brief Enumerates shot responses.
         */
        enum class ShotResponse {
            Missed, ///< The shot missed the target.
            Hit,    ///< The shot hit the target.
            Sunk,   ///< A ship has been sunk.
            Won     ///< The player has won the game.
        };

        // Network parameters
        ClientMode client_mode = ClientMode::Server;                ///< The client mode (Server or Client).
        std::string host = "localhost";                             ///< The network host address.
        uint16_t port = 8080;                                       ///< The network port number.
        NetworkState network_state = NetworkState::Unininitialized; ///< The current network state.
        bool compatible = false;                                    ///< Flag indicating network compatibility.
        std::string our_name = "Player";                            ///< The name of the local player.
        std::string their_name = "Enemy";                           ///< The name of the remote player.

        // Game parameters
        uint8_t width = 10;        ///< The width of the game grid.
        uint8_t height = 10;       ///< The height of the game grid.
        uint8_t n_battleships = 1; ///< The number of battleships in the game.
        uint8_t n_cruisers = 2;    ///< The number of cruisers in the game.
        uint8_t n_destroyers = 3;  ///< The number of destroyers in the game.
        uint8_t n_submarines = 4;  ///< The number of submarines in the game.

        // Events
        bool start_client_or_server = false;            ///< Flag indicating whether to start as a client or server.
        bool received_game_parameters = false;          ///< Flag indicating received game parameters.
        bool received_name = false;                     ///< Flag indicating received player name.
        bool timer_tick = false;                        ///< Flag indicating timer tick event.
        bool ready_to_begin = false;                    ///< Flag indicating readiness to begin the game.
        bool received_begin_confirmation = false;       ///< Flag indicating received begin confirmation.
        bool shoot = false;                             ///< Flag indicating a shot event.
        bool received_shot_response = false;            ///< Flag indicating received shot response.
        ShotResponse shot_other = ShotResponse::Missed; ///< Shot response from the opponent.
        bool received_shot = false;                     ///< Flag indicating received shot event.
        ShotResponse shot_self = ShotResponse::Missed;  ///< Shot response for the local player.
        bool disconnect = false;                        ///< Flag indicating a disconnect event.
        bool our_turn = false;                          ///< Flag indicating whether it's the local player's turn.

        // Shot parameters
        std::vector<std::pair<uint8_t, uint8_t>> sunk_positions = std::vector<std::pair<uint8_t, uint8_t>>(); ///< Positions of sunk ships.
        uint8_t shoot_x = 0;                                                                                  ///< X-coordinate of the shot.
        uint8_t shoot_y = 0;                                                                                  ///< Y-coordinate of the shot.
    };
}
