#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <vector>
#include <QObject>
#include "ui/zustand.h"
// Main game class for managing gameplay

/*!
 * \class Game
 * \brief Main game class for managing gameplay
 *
 * This class is responsible for handling the logic of the battleship game, including keeping track
 * of ship and enemy positions, checking for hits and losses, and interacting with the UI.
 */
class Game : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Default constructor that initializes an empty game state.
     */
    Game();

    /*!
     * \brief Overloaded constructor that initializes the game with a given set of ships, a board length, and a player turn flag.
     * \param ships A vector of integers representing the initial set of ship positions.
     * \param length The length of the side of the square game board.
     * \param playerOn A boolean flag indicating if it is currently the player's turn.
     */
    Game(const std::vector<int> &ships, const int length, const bool &playerOn);

    /*!
     * \brief Updates the position of an enemy entity on the UI game board. UI game board requires a flattered vector
     * \param position The position to update.
     * \return True if the position was updated successfully, false otherwise.
     */
    bool update_enemy_position(const int position);

    /*!
     * \brief Updates the position of an enemy entity on the game board using X and Y coordinates for the Server. Server requires X/Y coordinates
     * \param _x The X-coordinate of the position to update.
     * \param _y The Y-coordinate of the position to update.
     */
    void update_enemy_position(const int _x, const int _y);

    /*!
     * \brief Adds a ship to the game board.
     * \param position The starting position of the ship. For horizontal is from the left to the right and for vetical is from the top to the bottom.
     * \param shipType The type of ship to add.
     * \param orientation The orientation of the ship (horizontal or vertical).
     * \return True if the ship was added successfully, false otherwise.
     */
    bool add_ship(int position, const zustand::ship_kind &shipType, const zustand::Orientation &orientation);

    /*!
     * \brief Tests whether a ship can be placed at a given position.
     * \param position The starting position of the ship.
     * \param shipType The type of ship to test.
     * \param orientation The orientation of the ship (horizontal or vertical).
     * \return True if the ship can be placed at the given position, false otherwise.
     */
    bool test_place_ship(const int position, const zustand::ship_kind &shipType, const zustand::Orientation &orientation);

    /*!
     * \brief Gets the size of a ship based on its type.
     * \param shipType The type of ship.
     * \return The size of the ship.
     */
    int get_size(const zustand::ship_kind &shipType);

    /*!
     * \brief Tests whether a given position corresponds to a hit on the player's ships.
     * \param position The position to test.
     * \return True if the position corresponds to a hit, false otherwise.
     */
    bool test_if_hit(const int position);

    /*!
     * \brief Checks if the player has lost the game.
     * \return True if the player has lost, false otherwise.
     */
    bool lose() const;

    /*!
     * \brief Retrieves the positions of the player's ships for UI.
     * \return A vector of integers representing the positions of the player's ships.
     */
    std::vector<int> get_ships();

    /*!
     * \brief Get the list of ships with their respective positions.
     *
     * This method returns a vector where each element is a pair consisting of a ship kind and a vector of integers representing that ship's positions on the game board.
     *
     * \return A vector of pairs where each pair contains a ship kind and a vector of its positions on the game board.
     */
    std::vector<std::pair<zustand::ship_kind, std::vector<int>>> get_shipsList();

    /*!
     * \brief Set the dimensions of the game field.
     *
     * This method sets the width and height of the game board to the specified values.
     *
     * \param width The new width of the game board.
     * \param height The new height of the game board.
     */
    void set_field(const int width, const int height);

    /*!
     * \brief Get the positions affected by placing a ship.
     *
     * This method calculates and returns the positions on the game board that would be affected by placing a ship of the specified type and orientation at the specified position.
     *
     * \param position The starting position for the ship.
     * \param shipType The type of ship to place.
     * \param orientation The orientation of the ship.
     * \return A vector of integers representing the positions affected by placing the ship.
     */
    std::vector<int> take_positions(const int position, const zustand::ship_kind &shipType, const zustand::Orientation &orientation);

    /*!
     * \brief Add a ship to _shipList.
     *
     * This method adds a ship object consists of the specified type and its affected position to the list of ships.
     *
     * \param position The starting position for the ship.
     * \param shipType The type of ship to add.
     * \param orientation The orientation of the ship to add.
     */
    void add_ship_to_shipList(const int position, const zustand::ship_kind &shipType, const zustand::Orientation &orientation);

    /*!
     * \brief Convert coordinates to a single integer representing a position on the game board.
     *
     * This method takes two integers representing the x and y coordinates on the game board and converts them to a single integer representing a position on the game board.
     *
     * \param coordinateX The x coordinate to convert.
     * \param coordinateY The y coordinate to convert.
     * \return An integer representing the position on the game board corresponding to the specified coordinates.
     */
    int convert_coordinate(const int coordinateX, const int coordinateY);

    /*!
     * \brief Convert a position to x and y coordinates.
     *
     * This method takes an integer representing a position on the game board and converts it to x and y coordinates, which are then stored in the specified variables.
     *
     * \param pos The position to convert.
     * \param x Reference to a variable where the x coordinate will be stored.
     * \param y Reference to a variable where the y coordinate will be stored.
     */
    void convert_to_xy(const int pos, uint8_t &x, uint8_t &y);

    /*!
     * \brief Get the status of a specific cell on the game board.
     *
     * This method returns a tuple containing a string representing the status of the cell, the kind of ship at that cell (if any), and a vector of pairs representing the coordinates of all cells occupied by that ship.
     *
     * \param x The x coordinate of the cell to check.
     * \param y The y coordinate of the cell to check.
     * \return A tuple containing a string representing the cell's status, the kind of ship at that cell, and a vector of coordinate pairs for all cells occupied by that ship.
     */
    std::tuple<std::string, zustand::ship_kind, std::vector<std::pair<uint8_t, uint8_t>>> get_status(const int x, const int y);

public slots:

    void prepare_add_ship(QPushButton *pushed_button, zustand::ship_kind shipType, zustand::Orientation orientation);

    void reset_game();

signals:
    /*!
     * \brief Signal emitted to update the UI to a waiting state.
     *
     * This signal is emitted to update the UI to a waiting state.
     */
    void update_wait_ui();

    /*!
     * \brief Signal emitted when an invalid shot is fired.
     *
     * This signal is emitted when an invalid shot is detected.
     */
    void invalid_shot();

    /*!
     * \brief Signal emitted when a valid shot is fired.
     *
     * This signal is emitted when a valid shot is detected.
     */
    void valid_shot();

private:
    std::vector<int> _ships; ///< Vector holding the positions of the player's ships on the game board.
    std::vector<int> _enemy; ///< Vector holding the positions of the enemy entities on the game board.

    int _width = 10;  ///< Default width of the game board.
    int _height = 10; ///< Default height of the game board.

    bool _playerOn = false; ///< Boolean flag indicating whether it is the player's turn to play.

    std::vector<int> _map; ///< Vector representing the game board, with each cell holding a value representing its state (e.g., empty, contains a ship, has been shot).

    /*!
     * \brief List holding the player's ships along with their corresponding types and positions.
     *
     * This vector holds pairs where the first element is the kind of ship and the second element is a vector holding the positions of that ship on the game board.
     */
    std::vector<std::pair<zustand::ship_kind, std::vector<int>>> _shipsList;
};

#endif // GAME_HPP
