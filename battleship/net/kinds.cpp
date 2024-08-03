#include "kinds.h"

namespace kbm::net {
    std::vector<char> ParamMsg::serialize() const {
        auto buf = std::vector<char>();
        buf.push_back(static_cast<char>(width));
        buf.push_back(static_cast<char>(height));
        buf.push_back(static_cast<char>(n_battleships));
        buf.push_back(static_cast<char>(n_cruisers));
        buf.push_back(static_cast<char>(n_destroyers));
        buf.push_back(static_cast<char>(n_submarines));
        return buf;
    }

    std::optional<std::pair<ParamMsg, uint8_t>> ParamMsg::try_deserialize(const std::vector<char> &buf) {
        if (buf.size() >= 6) {
            auto msg = ParamMsg();
            msg.width = static_cast<uint8_t>(buf[0]);
            msg.height = static_cast<uint8_t>(buf[1]);
            msg.n_battleships = static_cast<uint8_t>(buf[2]);
            msg.n_cruisers = static_cast<uint8_t>(buf[3]);
            msg.n_destroyers = static_cast<uint8_t>(buf[4]);
            msg.n_submarines = static_cast<uint8_t>(buf[5]);
            return std::pair(std::move(msg), 6);
        }

        return {};
    }

    std::vector<char> BeginMsg::serialize() const {
        auto buf = std::vector<char>();
        return buf;
    }

    std::optional<std::pair<BeginMsg, uint8_t>>
    BeginMsg::try_deserialize([[maybe_unused]] const std::vector<char> &buf) {
        auto msg = BeginMsg();
        return std::pair(std::move(msg), 0);
    }

    std::vector<char> ShotMsg::serialize() const {
        auto buf = std::vector<char>();
        buf.push_back(static_cast<char>(x));
        buf.push_back(static_cast<char>(y));
        return buf;
    }

    std::optional<std::pair<ShotMsg, uint8_t>> ShotMsg::try_deserialize(const std::vector<char> &buf) {
        if (buf.size() >= 2) {
            auto msg = ShotMsg();
            msg.x = static_cast<uint8_t>(buf[0]);
            msg.y = static_cast<uint8_t>(buf[1]);
            return std::pair(std::move(msg), 2);
        }

        return {};
    }

    std::vector<char> StatusMsg::serialize() const {
        auto buf = std::vector<char>();
        buf.push_back(static_cast<char>(code));
        return buf;
    }

    std::optional<std::pair<StatusMsg, uint8_t>> StatusMsg::try_deserialize(const std::vector<char> &buf) {
        if (buf.size() >= 1) {
            auto msg = StatusMsg();
            msg.code = static_cast<StatusCode>(static_cast<uint8_t>(buf[0]));
            return std::pair(std::move(msg), 1);
        }

        return {};
    }

    std::vector<char> ResultMsg::serialize() const {
        auto buf = std::vector<char>();
        buf.push_back(static_cast<char>(code));
        if (code == ResultCode::HitSunk || code == ResultCode::HitWon) {
            for (auto pos : hits) {
                buf.push_back(static_cast<char>(pos.first));
                buf.push_back(static_cast<char>(pos.second));
            }
        }
        return buf;
    }

    std::optional<std::pair<ResultMsg, uint8_t>> ResultMsg::try_deserialize(const std::vector<char> &buf) {
        if (buf.size() >= 1) {
            auto msg = ResultMsg();
            msg.code = static_cast<ResultCode>(static_cast<uint8_t>(buf[0]));
            msg.hits = std::vector<std::pair<uint8_t, uint8_t>>();
            if (msg.code == ResultCode::HitSunk || msg.code == ResultCode::HitWon) {
                if (buf.size() >= 11) {
                    msg.hits.emplace_back(buf[9], buf[10]);
                    msg.hits.emplace_back(buf[7], buf[8]);
                    msg.hits.emplace_back(buf[5], buf[6]);
                    msg.hits.emplace_back(buf[3], buf[4]);
                    msg.hits.emplace_back(buf[1], buf[2]);
                    return std::pair(std::move(msg), 11);
                } else if (buf.size() >= 9) {
                    msg.hits.emplace_back(buf[7], buf[8]);
                    msg.hits.emplace_back(buf[5], buf[6]);
                    msg.hits.emplace_back(buf[3], buf[4]);
                    msg.hits.emplace_back(buf[1], buf[2]);
                    return std::pair(std::move(msg), 9);
                } else if (buf.size() >= 7) {
                    msg.hits.emplace_back(buf[5], buf[6]);
                    msg.hits.emplace_back(buf[3], buf[4]);
                    msg.hits.emplace_back(buf[1], buf[2]);
                    return std::pair(std::move(msg), 7);
                } else if (buf.size() >= 5) {
                    msg.hits.emplace_back(buf[3], buf[4]);
                    msg.hits.emplace_back(buf[1], buf[2]);
                    return std::pair(std::move(msg), 5);
                }
            } else {
                return std::pair(std::move(msg), 1);
            }
        }

        return {};
    }

    std::vector<char> IdMsg::serialize() const {
        auto buf = std::vector<char>();
        buf.push_back(static_cast<char>(id));
        return buf;
    }

    std::optional<std::pair<IdMsg, uint8_t>> IdMsg::try_deserialize(const std::vector<char> &buf) {
        if (buf.size() >= 1) {
            auto msg = IdMsg();
            msg.id = static_cast<uint8_t>(buf[0]);
            return std::pair(std::move(msg), 1);
        }

        return {};
    }

    std::vector<char> ConnectMsg::serialize() const {
        auto buf = std::vector<char>();
        buf.push_back(static_cast<char>(static_cast<uint8_t>(name.size())));
        for (uint8_t i = 0; i < name.size(); i++) {
            buf.push_back(name[i]);
        }
        return buf;
    }

    std::optional<std::pair<ConnectMsg, uint8_t>> ConnectMsg::try_deserialize(const std::vector<char> &buf) {
        if (buf.size() >= 1) {
            auto name_len = static_cast<uint8_t>(buf[0]);
            if (buf.size() >= 1u + name_len) {
                auto msg = ConnectMsg();

                std::string name = std::string();
                name.resize(name_len);
                for (uint8_t i = 0; i < name_len; i++) {
                    name[i] = buf[1 + i];
                }

                msg.name = name;
                return std::pair(std::move(msg), 1 + name_len);
            }
        }

        return {};
    }

    std::vector<char> ConfirmMsg::serialize() const {
        auto buf = std::vector<char>();
        buf.push_back(static_cast<char>(first));
        buf.push_back(static_cast<char>(static_cast<uint8_t>(name.size())));
        for (uint8_t i = 0; i < name.size(); i++) {
            buf.push_back(name[i]);
        }
        return buf;
    }

    std::optional<std::pair<ConfirmMsg, uint8_t>> ConfirmMsg::try_deserialize(const std::vector<char> &buf) {
        if (buf.size() >= 2) {
            auto name_len = static_cast<uint8_t>(buf[1]);
            if (buf.size() >= 2u + name_len) {
                auto msg = ConfirmMsg();

                std::string name = std::string();
                name.resize(name_len);
                for (uint8_t i = 0; i < name_len; i++) {
                    name[i] = buf[2 + i];
                }

                msg.first = static_cast<bool>(buf[0]);
                msg.name = name;
                return std::pair(std::move(msg), 2 + name_len);
            }
        }

        return {};
    }
}