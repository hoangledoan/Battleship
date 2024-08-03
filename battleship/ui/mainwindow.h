

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include "zustand.h"
#include <QPushButton>
#include "logic/game.hpp"
#include <memory>
#include "state.h"
#include <QMovie>
//#include <QMediaPlayer>

/**
 * @{ \name Field and Path constants
 */
/**
 * \brief Field fixed Sizes for later Demonstration and Background Path for button
 *
 * These constants represent the scale of UI and Path grey background img.
 */

#define MAIN_UI_WIDTH 571
#define MAIN_UI_HEIGHT 500

#define PLAY_UI_WIDTH 651
#define PLAY_UI_HEIGHT 500

#define PLAY_FIELD_PIX_WIDTH 500
#define PLAY_FIELD_PIX_HEIGHT 500

#define DEFAULT_GREY_PATH "background-image: url(:/Img_file/grey.jpg)"

/**
 * @}
 */

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
    class all_window;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

 /**
 * \brief Constructor of Main UI object
 *
 * Main UI consist of Menu UI, Wait UI, Play UI, as well as button field, selection buttons, text edit lines...
 *
 * \param shared_state Shared information with other branches for flow control, game play parameter...
 * \param playing_game Game Object control game logic and other state of the game.
 */
    explicit MainWindow(QWidget *parent = nullptr, std::shared_ptr<kbm::State> shared_state = nullptr, std::shared_ptr<Game> playing_game= nullptr);
    ~MainWindow();

public:
    // API set field from outside
    // API set list from outside

    /**
 * \brief Getter of wait field button vector.
 *
 *  Return reference to the wait field button vector for further modification.
 *
 *  \return button_container.
 */
    std::vector<QPushButton *> &get_button_container()
    {
        return button_container;
    }
 /**
 * \brief Getter of play field button vector.
 *
 *  Return reference to the play field button vector for further modification.
 *
 *  \return game_button_container.
 */
    std::vector<QPushButton *> &get_game_button_container()
    {
        return game_button_container;
    }

    /**
 * \brief Getter of play field button vector.
 *
 *  Methode to refresh display of #ship_to_place in Wait UI after every Ship placement.
 *  \sa prepare_add_ship(), update_wait_ui()
 */
    void update_wait_ui();

//    void display_missed_shot();

//    void display_sunk_shot();

//    void display_hitted_shot();
    /**
 * \brief Victory.
 *
 *  Some Animation before exit.
 *  \sa ui_control_flow()
 */
    void display_won();
    /**
 * \brief Defeated.
 *
 *  Some Animation before exit.
 *  \sa ui_control_flow()
 */
    void display_defeated();

public slots:
    // Transiton to wait ui, if play_mode== _server then always jump to wait ui
    //otherwise will wait till update ui being called, if return true then go to wait ui

    /**
 * \brief Start Application.
 *
 *  Click start after setup game mode/ #ships/ port/ ip...
 *  It automatically make sure that transition to Wati UI only happen after secured Connection has been established
 *  \sa
 */
    void on_start_clicked();

    /**
 * \brief Termination.
 *
 *  Uppon click Back button, the game will be forced to return to Menu UI, every connection will be immediately terminated,
 *  every game parms and display resources will be freed and reset to default.
 *  \sa reset_game(), abort()
 */
    // Slot for return to main_ui from waiting_ui and playing_ui
    void on_back_clicked();

    /**
 * \brief Ready To Play.
 *
 *  If every Ship has been placed, this methode will send out a signal to network to set mode to ready_to_begin
 *
 *  \sa ready_to_play()
 */
    // Transition to game_ui
    void on_play_clicked();
    /**
 * \brief Handle Click on Wait UI Button Field and Click on Play UI Button Field event
 *
 *  Depend on Widget( Wait Widget or Game Widget), the button will behave diffirently.
 *
 *  \sa prepare_add_ship(), shot_fired()
 */
    // Handle click event on play field
    void button_play_field_clicked();

    /**
 * \brief Prepare for Game UI
 *
 *  Button will be populate in this stage, where Info about field will be used to draw map.
 *
 */
    void start_game();


    /**
 * \brief Prepare for Wait UI
 *
 *  Button will be populate in this stage, where Info about field will be used to draw map.
 *  Here also where the Display board of ship and rotation to be seen, Ship muss be setted before start placing.
 *
 */
    //will be called after valid startup state
    void wait_setup();

    /**
 * \brief Get Play Parameters
 *
 * If Game run in Client mode, this Function Will be call to retreive Game Parameters from network and
 * store it in own init_stage.
 *
 */
    //after successful connection
    void get_play_parameters();

    /**
 * \brief Termination.
 *
 *  Uppon click Back button, the game will be forced to return to Menu UI, every connection will be immediately terminated,
 *  every game parms and display resources will be freed and reset to default.
 *  \sa reset_game(), abort()
 */
    void on_back2_clicked();
    /**
 * \brief Termination.
 *
 * If Network Connection is interrupted , this methode will be called to end the game immediately, reset all relevant Info to default.
 *
 *  \sa reset_game(), abort()
 */
    void forced_abort();

signals:
    /**
 * \brief Start Connection.
 *
 *  After start, try to get a live connection to start game, depend on mode then maybe get_parameter will be called.
 *
 *  \sa get_play_parameters()
 */
    void to_set_connection();

    /**
 * \brief Check placement.
 *
 *  In Wait Ui, when a button is clicked, the position will be check by game and if everything ok, display again newly placed ship on
 *  Wait UI
 *
 *  \sa button_play_field_clicked(), update_wait_ui()
 */
    void check_placement();

    /**
 * \brief Terminate.
 *
 *  Send active abort.
 *
 *  \sa on_back_clicked(), on_back2_clicked(), forced_abort()
 */
    void abort();


    /**
 * \brief Ready to Play.
 *
 *  Send out ready to play signal
 *
 *  \sa on_play_clicked()
 */
    void ready_to_play();

    /**
 * \brief Shot Signal.
 *
 *  Tell Network to deliver a shot.
 */
    void shot_fired();


private:
    // to be called when ever main_ui is requested
    /**
 * \brief App Configuration.
 *
 *  Setup some Animation and clear out all allocated memory in 2 containters.
 */
    void main_ui_config();

    /**
 * \brief Setup Menus.
 *
 *  Setup some Menus and it functionalities.
 */
    void menu_setup();

    /**
 * \brief Find Position on field of pushed Button
 *
 *  Naive Implementation.
 *
 *  \return plain Position(flattend)
 */
    int pos_pushed_button(QPushButton* pushed);

public:
    // for first time initialization

    zustand init_stage;///< Own state tracking struct

    /**
     * @{ \name Button Animations.
             */
         /**
 * \brief Animation for 3 Types of net response
 *
 * These constants represent the scale of UI and Path grey background img.
 */
    std::shared_ptr<QMovie> missed_animation;
    std::shared_ptr<QMovie> hitted_animation;
    std::shared_ptr<QMovie> destroyed_animation;
    //std::unique_ptr<QMediaPlayer> sound_track;
    /**
 * @}
 */


    std::vector<std::pair<int, uint8_t>> get_enemy;///< to match animation with button on play field

private:
    Ui::MainWindow *ui;///< Owner of static QWidget

    // menu and container passing down in gaming session

    std::unique_ptr<QMenu> mode_select;
    std::unique_ptr<QMenu> ship_input;
    std::unique_ptr<QMenu> rotation;

//    std::vector<uint16_t> nub_of_ships;
//    std::vector<uint16_t> connection_inf;
    /**
     * @{ \name Button Containers.
             */
    /**
 * \brief Button Containers.
 *
 * These containers will manage access and delete of manually generated buttons
 */
    std::vector<QPushButton *> button_container;
    std::vector<QPushButton *> game_button_container;
    /**
 * @}
 */

    std::shared_ptr<kbm::State>  shared_state;///< state of kabumm for communication
    std::shared_ptr<Game> current_game;///< direct acces to game logic
};

/**
* @brief doxy
*/

#endif // MAINWINDOW_H
