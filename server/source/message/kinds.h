#pragma once

#include <cstdint>
#include <utility>
#include <cstddef>
#include <optional>
#include <iostream>

namespace kbm {
    struct ParamMsg {
        static constexpr uint8_t cmd = 0x01;

        uint8_t width;
        uint8_t height;
        uint8_t n_battleships;
        uint8_t n_cruisers;
        uint8_t n_destroyers;
        uint8_t n_submarines;

        ParamMsg() = delete;
        ParamMsg(uint8_t width, uint8_t height, uint8_t n_battleships, uint8_t n_cruisers, uint8_t n_destroyers,
                 uint8_t n_submarines) :
                width(width), height(height), n_battleships(n_battleships), n_cruisers(n_cruisers),
                n_destroyers(n_destroyers), n_submarines(n_submarines) {};

        template<typename T>
        requires std::random_access_iterator<T>
        static std::optional<std::pair<ParamMsg, int64_t>> try_parse(T buf, int64_t len) {
            if (static_cast<uint8_t>(buf[0]) == cmd && len >= 7) {
                return std::pair(ParamMsg(
                        static_cast<uint8_t>(buf[1]),
                        static_cast<uint8_t>(buf[2]),
                        static_cast<uint8_t>(buf[3]),
                        static_cast<uint8_t>(buf[4]),
                        static_cast<uint8_t>(buf[5]),
                        static_cast<uint8_t>(buf[6])
                ), 7);
            }

            return {};
        }
    };

    struct BeginMsg {
        static constexpr uint8_t cmd = 0x02;

        BeginMsg() = default;

        template<typename T>
        static std::optional<std::pair<BeginMsg, int64_t>> try_parse(T buf, int64_t len __attribute__ ((unused))) {
            if (static_cast<uint8_t>(buf[0]) == cmd) {
                return std::pair(BeginMsg(), 1);
            }

            return {};
        }
    };

    struct ShotMsg {
        static constexpr uint8_t cmd = 0x03;

        uint8_t x;
        uint8_t y;

        ShotMsg() = delete;
        ShotMsg(uint8_t x, uint8_t y) : x(x), y(y) {}

        template<typename T>
        static std::optional<std::pair<ShotMsg, int64_t>> try_parse(T buf, int64_t len) {
            if (static_cast<uint8_t>(buf[0]) == cmd && len >= 3) {
                return std::pair(ShotMsg(
                        static_cast<uint8_t>(buf[1]),
                        static_cast<uint8_t>(buf[2])
                ), 3);
            }

            return {};
        }
    };

    struct StatusMsg {
        /*enum class Code : uint8_t {
            Ok = 0x00,
            NotReady = 0x01,
            NotSupported = 0x02,
            Incomplete = 0x03,
            Timeout = 0x04,
            OtherError = 0x20
        };*/

        static constexpr uint8_t cmd = 0x10;

        uint8_t code;

        StatusMsg() = delete;
        explicit StatusMsg(uint8_t code) : code(code) {}

        template<typename T>
        static std::optional<std::pair<StatusMsg, int64_t>> try_parse(T buf, int64_t len) {
            if (static_cast<uint8_t>(buf[0]) == cmd && len >= 2) {
                return std::pair(StatusMsg(
                        static_cast<uint8_t>(buf[1])
                ), 2);
            }

            return {};
        }
    };

    struct ResultMsg {
        static constexpr uint8_t cmd = 0x11;

        uint8_t code;

        ResultMsg() = delete;
        explicit ResultMsg(uint8_t code) : code(code) {}

        template<typename T>
        static std::optional<std::pair<ResultMsg, int64_t>> try_parse(T buf, int64_t len) {
            if (static_cast<uint8_t>(buf[0]) == cmd && len >= 2) {
                return std::pair(ResultMsg(
                        static_cast<uint8_t>(buf[1])
                ), 2);
            }

            return {};
        }

    };

    struct IdMsg {
        static constexpr uint8_t cmd = 0x80;

        uint8_t id;

        IdMsg() = delete;
        explicit IdMsg(uint8_t id) : id(id) {}

        template<typename T>
        static std::optional<std::pair<IdMsg, int64_t>> try_parse(T buf, int64_t len) {
            if (static_cast<uint8_t>(buf[0]) == cmd && len >= 2) {
                return std::pair(IdMsg(
                        static_cast<uint8_t>(buf[1])
                ), 2);
            }

            return {};
        }
    };

    struct ConnectMsg {
        static constexpr uint8_t cmd = 0x81;

        std::string name;

        ConnectMsg() = delete;
        explicit ConnectMsg(std::string name) : name(std::move(name)) {}

        template<typename T>
        static std::optional<std::pair<ConnectMsg, int64_t>> try_parse(T buf, int64_t len) {
            if (static_cast<uint8_t>(buf[0]) == cmd && len >= 2) {
                auto name_len = static_cast<uint8_t>(buf[1]);
                if (len >= name_len + 2) {
                    std::string name = std::string();
                    name.resize(name_len);
                    for (uint8_t i = 0; i < name_len; i++) {
                        name[i] = buf[2 + i];
                    }

                    return std::pair(ConnectMsg(name), 2 + name_len);
                }
            }

            return {};
        }
    };

    struct ConfirmMsg {
        static constexpr uint8_t cmd = 0x82;

        bool first;
        std::string name;

        ConfirmMsg() = delete;
        explicit ConfirmMsg(bool first, std::string name) : first(first), name(std::move(name)) {}

        template<typename T>
        static std::optional<std::pair<ConfirmMsg, int64_t>> try_parse(T buf, int64_t len) {
            if (static_cast<uint8_t>(buf[0]) == cmd && len >= 3) {
                auto first = static_cast<bool>(buf[1]);
                auto name_len = static_cast<uint8_t>(buf[2]);
                if (len >= name_len + 2) {
                    std::string name = std::string();
                    name.resize(name_len);
                    for (uint8_t i = 0; i < name_len; i++) {
                        name[i] = buf[3 + i];
                    }

                    return std::pair(ConfirmMsg(first, name), 3 + name_len);
                }
            }

            return {};
        }
    };

    struct UnknownMsg {
        UnknownMsg() = default;

    public:
        template<typename T>
        static std::optional<std::pair<UnknownMsg, int64_t>> try_parse(T buf __attribute__ ((unused)), int64_t len __attribute__ ((unused))) {
            return std::pair(UnknownMsg(), 1);
        }
    };
}
