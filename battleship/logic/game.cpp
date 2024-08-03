#include "game.hpp"
#include <algorithm>
#include <QDebug>
#include <QPushButton>

Game::Game() {}

Game::Game(const std::vector<int> &ships, const int length, const bool &playerOn)
    : _ships(ships), _playerOn(playerOn), _map(length * length){};

bool Game::update_enemy_position(const int position)
{
    // Validation: Assuming positions must be between 0 and (_length * _length - 1)
    if (position < 0 || position >= (_width * _height))
    {
        std::cout << "Invalid enemy position!\n";
        return false;
    }

    // Check for duplicates
    if (std::find(_enemy.begin(), _enemy.end(), position) != _enemy.end())
    {
        std::cout << "Duplicate enemy position!\n";

        return false;
    }

    _enemy.push_back(position);

    // Placeholder for rendering UI or other actions
    return true;
}

int Game::get_size(const zustand::ship_kind &shipType)
{
    static int size = 0;
    // Check type of ship
    if (shipType == zustand::ship_kind::Kreuzer)
    {
        size = 4;
    }
    else if (shipType == zustand::ship_kind::Schlachtschiff)
    {
        size = 5;
    }
    else if (shipType == zustand::ship_kind::Zerstoerer)
    {
        size = 3;
    }

    else
    {
        size = 2;
    };

    return size;
};

bool Game::test_place_ship(const int position, const zustand::ship_kind &shipType, const zustand::Orientation &orientation)
{
    int row = position / _width;
    int column = position % _height;
    int size = get_size(shipType);
    // check if the position is in the range
    if (row >= _width)
        return false;
    if (column >= _height)
        return false;

    if (orientation == zustand::Orientation::_horizontal)
    {
        int endColumn = column + size;
        if (endColumn > _width)
            return false;
    }
    else
    {
        int endRow = row + size;
        if (endRow > _height)
            return false;
    }

    return true;
}
std::vector<int> Game::take_positions(const int position, const zustand::ship_kind &shipType, const zustand::Orientation &orientation)
{
    int row = position / _width;
    int column = position % _width;
    int size = get_size(shipType);

    // check if the position is valid within the map first
    if (!test_place_ship(position, shipType, orientation))
    {
        std::cout << "Failed!\n";
        return std::vector<int>{};
    }

    // create a vector of the ship
    std::vector<int> shipPositions;
    if (orientation == zustand::Orientation::_horizontal)
    {
        for (int c = column; c < column + size; c++)
        {
            int shipPosition = row * _width + c;
            shipPositions.push_back(shipPosition);
        }
    }
    else // orientation == zustand::Orientation::vertical
    {
        for (int r = row; r < row + size; r++)
        {
            int shipPosition = r * _height + column;
            shipPositions.push_back(shipPosition);
        }
    }
    return shipPositions;
};

bool Game::add_ship(int position, const zustand::ship_kind &shipType, const zustand::Orientation &orientation)
{

    auto shipPositions = take_positions(position, shipType, orientation);
    // check if any part of the ship is in the _ships vector

    // if return null vec then false
    if (shipPositions.size() == 0)
        return false;

    for (int shipPosition : shipPositions)
    {
        if (std::find(_ships.begin(), _ships.end(), shipPosition) != _ships.end())
        {
            std::cout << "Position taken!\n";
            return false;
        }
    }

    // if not, add the ship vector to the _ships vector
    _ships.insert(_ships.end(), shipPositions.begin(), shipPositions.end());
    std::cout << "Succeed!\n";
    return true;
}

// Check if the player is hitted. If yes remove that position from _ships
// bool Game::test_if_hit(const int position)
//{
//    // Look for the position in the _ships vector
//    auto it = std::find(_ships.begin(), _ships.end(), position);

//    if (it != _ships.end()) // If position was found
//    {
//        // Remove the position from the _ships vector
//        _ships.erase(it);

//        std::cout << "Hitted!\n";
//        return true;
//    }

//    std::cout << "Missed!\n";
//    return false;
//};

std::vector<int> Game::get_ships()
{
    return _ships;
}

// Checks if the player has lost the game
bool Game::lose() const
{
    return _ships.empty();
}

void Game::prepare_add_ship(QPushButton *pushed_button, zustand::ship_kind shipType, zustand::Orientation orientation)
{

    qDebug() << pushed_button->text();

    int position = pushed_button->text().toInt();

    if (add_ship(position, shipType, orientation))
    {
        emit update_wait_ui();
    }
}

void Game::set_field(const int width, const int height)
{
    _width = width;
    _height = height;
}

void Game::add_ship_to_shipList(const int position, const zustand::ship_kind &shipType, const zustand::Orientation &orientation)
{
    if (add_ship(position, shipType, orientation))
    {
        auto shipPositions = take_positions(position, shipType, orientation);
        auto ship = std::make_pair(shipType, shipPositions);
        _shipsList.push_back(ship);
        emit update_wait_ui();
    }
}

void Game::update_enemy_position(const int _x, const int _y)
{
    _enemy.push_back(_x + _y * _width);
}

void Game::reset_game()
{
    _ships.clear();
    _enemy.clear();
    _width = 10;
    _height = 10;
    _playerOn = false;
    _map.clear();
}

int Game::convert_coordinate(const int coordinateX, const int coordinateY)
{
    // Check for valid
    if (coordinateX < 0 || coordinateX >= _width || coordinateY < 0 || coordinateY >= _height)
    {
        std::cerr << "Coordinates out of bounds!\n";
        return -1;
    }

    return coordinateY * _width + coordinateX;
}

void Game::convert_to_xy(const int pos, uint8_t &x, uint8_t &y)
{
    x = pos % _width;
    y = static_cast<uint8_t>(pos / _width);
}

std::tuple<std::string, zustand::ship_kind, std::vector<std::pair<uint8_t, uint8_t>>> Game::get_status(const int x, const int y)
{
    int position = convert_coordinate(x, y);
    // Check if the position is valid
    if (position < 0 || position >= (_width * _height))
    {
        return {"Invalid", zustand::ship_kind::undefined, {}};
    }

    // Create a copy of _shipsList to modify
    auto copiedShipsList = _shipsList;

    // Search for a ship that contains the given position in the copy
    for (auto it = copiedShipsList.begin(); it != copiedShipsList.end(); ++it)
    {
        std::vector<int> &shipPositions = it->second;

        // Check if the ship vector contains the position
        auto positionIter = std::find(shipPositions.begin(), shipPositions.end(), position);
        if (positionIter != shipPositions.end())
        {
            // Check if it's the last position in the ship's vector
            if (std::next(positionIter) == shipPositions.end())
            {
                std::pair<zustand::ship_kind, std::vector<int>> sunkShip = *it;

                // Remove the object from the original _shipsList
                _shipsList.erase(std::remove_if(_shipsList.begin(), _shipsList.end(),
                                                [&sunkShip](const std::pair<zustand::ship_kind, std::vector<int>> &pair)
                                                {
                                                    return pair == sunkShip;
                                                }),
                                 _shipsList.end());

                std::vector<std::pair<uint8_t, uint8_t>> positions;
                positions.resize(sunkShip.second.size());
                std::transform(sunkShip.second.begin(), sunkShip.second.end(), positions.begin(), [this](int pos)
                               {
                    uint8_t x, y;
                    this->convert_to_xy(pos, x, y);
                    return std::pair(x, y); });

                // Ship was sunk
                return {"Sunk", sunkShip.first, positions};
            }
            else
            {
                // Ship was hit but not sunk
                return {"Hit", zustand::ship_kind::undefined, {}};
            }
        }
    }

    // Check if _shipsList is empty, announce Lost
    if (_shipsList.empty())
    {
        return {"Lost", zustand::ship_kind::undefined, {}};
    }

    // If no ship was found, return missed
    return {"Missed", zustand::ship_kind::undefined, {}};
}

std::vector<std::pair<zustand::ship_kind, std::vector<int>>> Game::get_shipsList()
{
    return _shipsList;
}