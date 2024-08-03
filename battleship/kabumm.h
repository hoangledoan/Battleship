#ifndef KABUMM_H
#define KABUMM_H

#include "state.h"
#include <QObject>
#include "ui/mainwindow.h"
#include "logic/game.hpp"
#include "net/network.h"
#include <memory>
#include <QTimer>

using namespace kbm::net;

/**
 * @class Kabumm
 * @brief The main class representing the Kabumm game application.
 *
 * This class serves as the core component of the Kabumm game application. It manages game state,
 * user interface, network communication, and various game-related functionalities.
 */
class Kabumm : public QObject {
Q_OBJECT

public:
    /**
     * @brief Constructor for the Kabumm class.
     * @param parent A pointer to the parent QObject.
     */
    explicit Kabumm(QObject *parent = nullptr);

    /**
     * @brief Destructor for the Kabumm class.
     *
     * This destructor cleans up any dynamically allocated resources, such as the main UI.
     */
    ~Kabumm() {
        delete main_ui;
    }

private slots:

    /**
     * @brief Slot for updating the user interface during waiting periods.
     */
    void update_wait_ui();

    /**
     * @brief Slot for checking the placement of game elements.
     */
    void check_placement();

    /**
     * @brief Slot for controlling the flow of the user interface.
     */
    void ui_flow_control();

public:
    /**
     * @brief Shared pointer to the game state.
     */
    std::shared_ptr<kbm::State> state;

    /**
     * @brief Shared pointer to the game instance.
     */
    std::shared_ptr<Game> new_game;

    /**
     * @brief Pointer to the main application window.
     */
    MainWindow *main_ui;

    /**
     * @brief Unique pointer to the network communication module.
     */
    std::unique_ptr<Network> network;

    /**
     * @brief Unique pointer to a QTimer for recurring network events.
     */
    std::unique_ptr<QTimer> timer = std::make_unique<QTimer>();
};

#endif // KABUMM_H
