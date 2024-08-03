#pragma once

#ifndef ZUSTAND_H
#define ZUSTAND_H
#include <map>
#include <cstdint>
#include <QString>
#include <QPushButton>
class zustand
{

public:
    zustand() {}

    /**
 * \brief Enum ship_kind
 *
 *  For clarity
 */
    enum class ship_kind
    {
        Schlachtschiff,
        Kreuzer,
        Zerstoerer,
        UBoot,
        undefined
    };

    /**
 * \brief Enum SERVER_CLIENT
 *
 *  For clarity
 */
    enum class SERVER_CLIENT
    {
        _server,
        _client
    };

    /**
 * \brief Enum Orientation
 *
 *  For clarity
 */
    enum class Orientation
    {
        _vertical,
        _horizontal
    };

    /**
 * \brief Field Size
 *
 *  Store size of field.
 *  \param width
 *  \param height
 */
    class field
    {
    public:
        field() {}

        uint16_t width = 10;
        uint16_t height = 10;
    };

    /**
 * \brief Ship List
 *
 *  Using map to manage Ship List for Display in Wait UI
 *
 */
    std::map<ship_kind, uint16_t> ship_list{{ship_kind::Schlachtschiff, 1}, {ship_kind::Kreuzer, 0}, {ship_kind::Zerstoerer, 0}, {ship_kind::UBoot, 0}};

    ship_kind on_edit = ship_kind::undefined;///<Tempolary store edited ship kind
    field play_field;

    //
    QString server_ip;///<Tempolary store Server_ip
    uint16_t server_port;///<Tempolary store Server_port

    // keep tracking orientation mode
    Orientation or_mode = Orientation::_horizontal;///<Tempolary store Orientation mode

    // keep tracking operation mode as server or client
    SERVER_CLIENT op_mode = SERVER_CLIENT::_server;///<Tempolary store Operation Mode
    QPushButton *pushed_button = nullptr;///<Tempolary store newly clicked pushed button
    int pos_pushed;///<Tempolary store Position of pushed button.
};

#endif // ZUSTAND_H
