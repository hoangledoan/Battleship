
#include "kabumm.h"

Kabumm::Kabumm(QObject *parent) : QObject{parent},
                                  state(std::make_shared<kbm::State>()),
                                  new_game(std::make_shared<Game>()),
                                  main_ui(new MainWindow(nullptr, state, new_game)),
                                  network(std::make_unique<Network>(state, new_game)) {
    timer->start(500);

    connect(timer.get(), &QTimer::timeout, this, [this]() {
        this->state->timer_tick = true;
        this->network->handle_event();
    });

    // on first established server/client
    connect(main_ui, &MainWindow::to_set_connection, network.get(), &Network::handle_event);

    // placement check
    connect(main_ui, &MainWindow::check_placement, this, &Kabumm::check_placement);
    connect(new_game.get(), &Game::update_wait_ui, this, &Kabumm::update_wait_ui);

    // abort event from user tell network
    connect(main_ui, &MainWindow::abort, network.get(), &Network::handle_event);

    // wait ready and begin getting in queue to play
    connect(main_ui, &MainWindow::ready_to_play, network.get(), &Network::handle_event);

    // abort to reset gameplay
    connect(main_ui, &MainWindow::abort, new_game.get(), &Game::reset_game);

    // on shot from us fired check legitim

    // continue second part of sent out signal from game to network if successed

    connect(main_ui, &MainWindow::shot_fired, network.get(), &Network::handle_event);

    // upon hit/ miss, destroy animation
    connect(network.get(), &Network::update_ui, this, &Kabumm::ui_flow_control);
}

void Kabumm::ui_flow_control() {
    using namespace kbm;
    if (state->received_game_parameters) {
        // reset check receive_game_parameters
        state->received_game_parameters = false;
        main_ui->get_play_parameters();
        if (state->received_name) {
            // display enemy name
        }
    } else if (state->received_begin_confirmation) {
        // reset check receive_begin_confirmation
        state->received_begin_confirmation = false;
        main_ui->start_game();
    } else if (state->received_shot_response) {
        // reset received_shot_response
        state->received_shot_response = false;

        if (state->shot_other == State::ShotResponse::Missed) {
            main_ui->get_enemy.push_back(std::pair<int, uint8_t>{main_ui->init_stage.pos_pushed, 1});
        } else if (state->shot_other == State::ShotResponse::Hit) {
            main_ui->get_enemy.push_back(std::pair<int, uint8_t>{main_ui->init_stage.pos_pushed, 2});
        } else if (state->shot_other == State::ShotResponse::Sunk) {
            main_ui->get_enemy.push_back(std::pair<int, uint8_t>{main_ui->init_stage.pos_pushed, 3});
        }

        disconnect(main_ui->hitted_animation.get(), 0, 0, 0);

        for (auto &x: main_ui->get_enemy) {
            main_ui->get_game_button_container()[x.first]->setStyleSheet("");
            if (x.second == 1) {
                connect(main_ui->missed_animation.get(), &QMovie::frameChanged, this,
                        [this, x] { main_ui->get_game_button_container()[x.first]->setIcon(main_ui->missed_animation->currentPixmap()); });
            } else if (x.second == 2) {
                bool to_make_connect = true;

                for (auto &y: state->sunk_positions) {

                    if (new_game->convert_coordinate(y.first, y.second) == x.first) {
                        to_make_connect = false;
                        break;
                    }
                }
                if (to_make_connect) {
                    connect(main_ui->hitted_animation.get(), &QMovie::frameChanged, this, [this, x] {
                        main_ui->get_game_button_container()[x.first]->setIcon(main_ui->hitted_animation->currentPixmap());
                    });
                } else {
                    disconnect(main_ui->hitted_animation.get(), 0,
                               main_ui->get_game_button_container()[x.first], 0);
                }
            }
        }

        for (auto &x: state->sunk_positions) {
            connect(main_ui->destroyed_animation.get(), &QMovie::frameChanged, this, [this, x] {
                main_ui->get_game_button_container()[new_game->convert_coordinate(x.first, x.second)]
                        ->setIcon(main_ui->destroyed_animation->currentPixmap());
            });
        }

    }
    if (state->disconnect) {
        state->disconnect = false;
        main_ui->forced_abort();
    }
}

void Kabumm::update_wait_ui() {
    --main_ui->init_stage.ship_list[main_ui->init_stage.on_edit];

    for (auto x: new_game->get_shipsList()) {
        for (auto pos: x.second) {
            main_ui->get_button_container()[pos]->setStyleSheet("background: rgb(0, 0, 255);");
        }
    }
    main_ui->update_wait_ui();
}

void Kabumm::check_placement() {
    new_game->add_ship_to_shipList(main_ui->init_stage.pos_pushed, main_ui->init_stage.on_edit, main_ui->init_stage.or_mode);
}
