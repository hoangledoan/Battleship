#pragma once

#include "message.h"

#include <string>
#include <utility>

namespace kbm::net {
    /**
     * @brief Message for negotiating game parameters (ID 0x01)
     */
    class ParamMsg : public Message {
    public:
        uint8_t width; /**< @brief Game field width */
        uint8_t height; /**< @brief Game field height */
        uint8_t n_battleships; /**< @brief Number of battleships (5-long ships) to place */
        uint8_t n_cruisers; /**< @brief Number of cruisers (4-long ships) to place */
        uint8_t n_destroyers; /**< @brief Number of destroyers (3-long ships) to place */
        uint8_t n_submarines; /**< @brief Number of submarines (2-long ships) to place */

        uint8_t cmd() const override { return 0x01; };

        std::vector<char> serialize() const override;

        /**
         * @brief Attempt to serialise this message type from a byte vector (without the command-id)
         * @param buf The byte vector to parse (without the command id)
         * @returns The parsed message and its length, if parsing succeeded, and nothing otherwise
         */
        static std::optional<std::pair<ParamMsg, uint8_t>> try_deserialize(const std::vector<char> &buf);

        /**
         * @brief Construct this message type from its arguments
         * @param width Game field width
         * @param height Game field height
         * @param n_battleships Number of battleships (5-long ships) to place
         * @param n_cruisers Number of cruisers (4-long ships) to place
         * @param n_destroyers Number of destroyers (3-long ships) to place
         * @param n_submarines Number of submarines (2-long ships) to place
         */
        explicit ParamMsg(uint8_t width = 0x00, uint8_t height = 0x00, uint8_t n_battleships = 0x00, uint8_t n_cruisers = 0x00, uint8_t n_destroyers = 0x00,
                          uint8_t n_submarines = 0x00)
                : width(width), height(height), n_battleships(n_battleships), n_cruisers(n_cruisers), n_destroyers(n_destroyers), n_submarines(n_submarines) {}
    };

    /**
     * @brief Message for announcing readiness to begin the game (ID 0x02)
     */
    class BeginMsg : public Message {
    public:
        uint8_t cmd() const override { return 0x02; };

        std::vector<char> serialize() const override;

        /**
         * @brief Attempt to serialise this message type from a byte vector (without the command-id)
         * @param buf The byte vector to parse (without the command id)
         * @returns The parsed message and its length, if parsing succeeded, and nothing otherwise
         */
        static std::optional<std::pair<BeginMsg, uint8_t>> try_deserialize([[maybe_unused]] const std::vector<char> &buf);

        /**
         * @brief Construct this message type from its arguments
         */
        BeginMsg() = default;
    };

    /**
     * @brief Message for requesting a shot at a position (ID 0x03)
     */
    class ShotMsg : public Message {
    public:
        uint8_t x; /**< @brief X-position of the shot */
        uint8_t y; /**< @brief Y-position of the shot */

        uint8_t cmd() const override { return 0x03; };

        std::vector<char> serialize() const override;

        /**
         * @brief Attempt to serialise this message type from a byte vector (without the command-id)
         * @param buf The byte vector to parse (without the command id)
         * @returns The parsed message and its length, if parsing succeeded, and nothing otherwise
         */
        static std::optional<std::pair<ShotMsg, uint8_t>> try_deserialize(const std::vector<char> &buf);

        /**
         * @brief Construct this message type from its arguments
         * @param x X-position of the shot
         * @param y Y-position of the shot
         */
        explicit ShotMsg(uint8_t x = 0, uint8_t y = 0) : x(x), y(y) {}
    };

    /**
     * @brief Message for answering a shot request (ID 0x10)
     */
    class StatusMsg : public Message {
    public:
        enum StatusCode : uint8_t {
            Ok = 0x00,
            NotReady = 0x01,
            NotSupported = 0x02,
            Incomplete = 0x03,
            Timeout = 0x04,
            OtherError = 0x20
        };

        StatusCode code; /**< @brief A StatusCode with the result of a requested operation */

        uint8_t cmd() const override { return 0x10; };

        std::vector<char> serialize() const override;

        /**
         * @brief Attempt to serialise this message type from a byte vector (without the command-id)
         * @param buf The byte vector to parse (without the command id)
         * @returns The parsed message and its length, if parsing succeeded, and nothing otherwise
         */
        static std::optional<std::pair<StatusMsg, uint8_t>> try_deserialize(const std::vector<char> &buf);

        /**
         * @brief Construct this message type from its arguments
         * @param code A StatusCode with the result of a requested operation
         */
        explicit StatusMsg(StatusCode code = StatusCode::Ok) : code(code) {}
    };

    /**
     * @brief Message for answering a begin request (ID 0x11)
     */
    class ResultMsg : public Message {
    public:
        enum ResultCode : uint8_t {
            Missed = 0x00,
            Hit = 0x01,
            HitSunk = 0x02,
            HitWon = 0x03,
            NotYourTurn = 0x10,
            OutsideField = 0x11,
            OtherError = 0x20,
        };

        ResultCode code; /**< @brief A ResultCode with the result of a requested operation */
        std::vector<std::pair<uint8_t, uint8_t>> hits; /**< @brief If the code is HitSunk or HitWon: All positions of the sunk ship */

        uint8_t cmd() const override { return 0x11; };

        std::vector<char> serialize() const override;

        /**
         * @brief Attempt to serialise this message type from a byte vector (without the command-id)
         * @param buf The byte vector to parse (without the command id)
         * @returns The parsed message and its length, if parsing succeeded, and nothing otherwise
         */
        static std::optional<std::pair<ResultMsg, uint8_t>> try_deserialize(const std::vector<char> &buf);

        /**
         * @brief Construct this message type from its arguments
         * @param code A ResultCode with the result of a requested operation
         * @param hits If the code is HitSunk or HitWon: All positions of the sunk ship
         */
        explicit ResultMsg(ResultCode code = ResultCode::Missed, std::vector<std::pair<uint8_t, uint8_t>> hits = std::vector<std::pair<uint8_t, uint8_t>>()) : code(code), hits(std::move(hits)) {}
    };

    /**
     * @brief Message for announcing a client ID (ID 0x80)
     */
    class IdMsg : public Message {
    public:
        uint8_t id; /**< @brief Client/group ID (0x03 for this client) */

        uint8_t cmd() const override { return 0x80; };

        std::vector<char> serialize() const override;

        /**
         * @brief Attempt to serialise this message type from a byte vector (without the command-id)
         * @param buf The byte vector to parse (without the command id)
         * @returns The parsed message and its length, if parsing succeeded, and nothing otherwise
         */
        static std::optional<std::pair<IdMsg, uint8_t>> try_deserialize(const std::vector<char> &buf);

        /**
         * @brief Construct this message type from its arguments
         * @param id Client/group ID (0x03 for this client)
         */
        explicit IdMsg(uint8_t id = 0x00) : id(id) {}
    };

    /**
     * @brief Message for connecting to matchmaking server or exchanging user names with a client (ID 0x81)
     */
    class ConnectMsg : public Message {
    public:
        std::string name; /**< @brief Name of the player */

        uint8_t cmd() const override { return 0x81; };

        std::vector<char> serialize() const override;

        /**
         * @brief Attempt to serialise this message type from a byte vector (without the command-id)
         * @param buf The byte vector to parse (without the command id)
         * @returns The parsed message and its length, if parsing succeeded, and nothing otherwise
         */
        static std::optional<std::pair<ConnectMsg, uint8_t>> try_deserialize(const std::vector<char> &buf);

        /**
         * @brief Construct this message type from its arguments
         * @param name Name of the player
         */
        explicit ConnectMsg(std::string name = "") : name(std::move(name)) {}
    };

    /**
     * @brief Message for answering a matchmaking request (ID 0x82)
     */
    class ConfirmMsg : public Message {
    public:
        bool first; /**< @brief This client has the first turn */
        std::string name; /**< @brief Name of the opposing player */

        uint8_t cmd() const override { return 0x82; };

        std::vector<char> serialize() const override;

        /**
         * @brief Attempt to serialise this message type from a byte vector (without the command-id)
         * @param buf The byte vector to parse (without the command id)
         * @returns The parsed message and its length, if parsing succeeded, and nothing otherwise
         */
        static std::optional<std::pair<ConfirmMsg, uint8_t>> try_deserialize(const std::vector<char> &buf);

        /**
         * @brief Construct this message type from its arguments
         * @param first This client has the first turn
         * @param name Name of the opposing player
         */
        explicit ConfirmMsg(bool first = true, std::string name = "") : first(first), name(std::move(name)) {}
    };
}