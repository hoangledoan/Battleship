#pragma once

#include "kinds.h"
#include "concepts.h"

#include <iostream>

namespace kbm {
    template<typename STATE>
    class Message {
    public:
        Message() = delete;
        Message(const Message&) = delete;
    };

    class Unsafe;
    template<typename KIND>
    requires CanTryParse<KIND>
    class Safe;

    using UnsafeMsg = Message<Unsafe>;
    using SafeMsg = std::variant<
            Message<Safe<ParamMsg>>,
            Message<Safe<BeginMsg>>,
            Message<Safe<ShotMsg>>,
            Message<Safe<StatusMsg>>,
            Message<Safe<ResultMsg>>,
            Message<Safe<IdMsg>>,
            Message<Safe<ConnectMsg>>,
            Message<Safe<ConfirmMsg>>,
            Message<Safe<UnknownMsg>>
    >;

    template<typename KIND>
    requires CanTryParse<KIND>
    class Message<Safe<KIND>> {
        friend class Message<Unsafe>;

        KIND msg;

        explicit Message(KIND msg) : msg(msg) {};
    public:
        Message() = delete;

        auto operator*() -> KIND& {
            return msg;
        }
    };

    template<>
    class Message<Unsafe> {
        std::vector<char> buffer;

    public:
        explicit Message(std::vector<char>&& buffer) : buffer(buffer) {};

        std::vector<SafeMsg> parse() const {
            auto messages = std::vector<SafeMsg>();
            int64_t index = 0;

            while (static_cast<size_t>(index) < buffer.size()) {
                if (auto res = ParamMsg::try_parse(buffer.begin() + index, buffer.size() - index); res) {
                    auto [msg, len] = res.value();
                    messages.emplace_back(Message<Safe<ParamMsg>>(msg));
                    index += len;
                } else if (auto res = BeginMsg::try_parse(buffer.begin() + index, buffer.size() - index); res) {
                    auto [msg, len] = res.value();
                    messages.emplace_back(Message<Safe<BeginMsg>>(msg));
                    index += len;
                } else if (auto res = ShotMsg::try_parse(buffer.begin() + index, buffer.size() - index); res) {
                    auto [msg, len] = res.value();
                    messages.emplace_back(Message<Safe<ShotMsg>>(msg));
                    index += len;
                } else if (auto res = StatusMsg::try_parse(buffer.begin() + index, buffer.size() - index); res) {
                    auto [msg, len] = res.value();
                    messages.emplace_back(Message<Safe<StatusMsg>>(msg));
                    index += len;
                } else if (auto res = ResultMsg::try_parse(buffer.begin() + index, buffer.size() - index); res) {
                    auto [msg, len] = res.value();
                    messages.emplace_back(Message<Safe<ResultMsg>>(msg));
                    index += len;
                } else if (auto res = IdMsg::try_parse(buffer.begin() + index, buffer.size() - index); res) {
                    auto [msg, len] = res.value();
                    messages.emplace_back(Message<Safe<IdMsg>>(msg));
                    index += len;
                } else if (auto res = ConnectMsg::try_parse(buffer.begin() + index, buffer.size() - index); res) {
                    auto [msg, len] = res.value();
                    messages.emplace_back(Message<Safe<ConnectMsg>>(msg));
                    index += len;
                } else if (auto res = ConfirmMsg::try_parse(buffer.begin() + index, buffer.size() - index); res) {
                    auto [msg, len] = res.value();
                    messages.emplace_back(Message<Safe<ConfirmMsg>>(msg));
                    index += len;
                } else if (auto res = UnknownMsg::try_parse(buffer.begin() + index, buffer.size() - index); res) {
                    auto [msg, len] = res.value();
                    messages.emplace_back(Message<Safe<UnknownMsg>>(msg));
                    index += len;
                } else {
                    assert(false);
                }
            }

            return messages;
        }
    };
}