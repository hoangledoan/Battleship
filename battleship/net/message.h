#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <QDataStream>
#include <array>

namespace kbm::net {
    /**
     * @brief An abstract safe Message parsed from a TCP-Packet or generated by the game
     */
    class Message {
    public:
        virtual ~Message() = default;

        /**
         * @brief Get the command-id of a specific message type
         * @returns A single-byte command-id
         */
        virtual uint8_t cmd() const = 0;

        /**
         * @brief Serialize a specific message type
         * @returns A byte vector with the serialised message
         */
        virtual std::vector<char> serialize() const = 0;

        /**
         * @brief Write a single Message to a byte vector
         * @returns A byte vector with the serialised message
         */
        std::vector<char> out() const;

        /**
         * @brief Read multiple Messages from a byte vector
         * @param buf A byte vector possibly containing multiple messages
         * @param enable_extensions Enable parsing of non-standard protocol extensions
         * @returns A vector of Messages on the heap
         */
        static std::vector<std::unique_ptr<Message>> in(const std::vector<char>& buf, bool enable_extensions = false);
    };

    /**
     * @brief Write a Message directly to a QDataStream
     * @param stream The stream to write to
     * @param msg Any single Message
     * @returns The given stream
     */
    QDataStream& operator<<(QDataStream& stream, const kbm::net::Message& msg);
}

