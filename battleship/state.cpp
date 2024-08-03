#include "state.h"

#include <iostream>

namespace kbm {
    void State::debug_print() const {
        std::cout << "State {" << std::endl;
        std::cout << "\tstart_client_or_server: " << start_client_or_server << std::endl;
        std::cout << "\treceived_game_parameters: " << received_game_parameters << std::endl;
        std::cout << "\treceived_name: " << received_name << std::endl;
        std::cout << "\ttimer_tick: " << timer_tick << std::endl;
        std::cout << "\tready_to_begin: " << ready_to_begin << std::endl;
        std::cout << "\treceived_begin_confirmation: " << received_begin_confirmation << std::endl;
        std::cout << "\tshoot: " << shoot << std::endl;
        std::cout << "\treceived_shot_response: " << received_shot_response << std::endl;
        std::cout << "\treceived_shot: " << received_shot << std::endl;
        std::cout << "\tdisconnect: " << disconnect << std::endl;
        std::cout << std::endl;
        std::cout << "\tnetwork_state: ";
        switch (network_state) {
            case NetworkState::Unininitialized: {
                std::cout << "Unininitialized";
                break;
            }
            case NetworkState::WaitingForConnection: {
                std::cout << "WaitingForConnection";
                break;
            }
            case NetworkState::WaitingForParams: {
                std::cout << "WaitingForParams";
                break;
            }
            case NetworkState::WaitingForBegin: {
                std::cout << "WaitingForBegin";
                break;
            }
            case NetworkState::InGame: {
                std::cout << "InGame";
                break;
            }
        }
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "\thost: " << host << std::endl;
        std::cout << "\tport: " << port << std::endl;
        std::cout << std::endl;
        std::cout << "\twidth: " << static_cast<uint32_t>(width) << std::endl;
        std::cout << "\theight: " << static_cast<uint32_t>(height) << std::endl;
        std::cout << "}" << std::endl;
    }
}
