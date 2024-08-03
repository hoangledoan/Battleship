#include "message.h"
#include "kinds.h"

namespace kbm::net {
    std::vector<char> Message::out() const {
        std::vector<char> buf = serialize();
        buf.insert(buf.begin(), static_cast<char>(cmd()));
        return buf;
    }

    std::vector<std::unique_ptr<Message>> Message::in(const std::vector<char> &buf, const bool enable_extensions) {
        auto msgs = std::vector<std::unique_ptr<Message>>();
        size_t index = 0;

        while (index < buf.size()) {
            if (static_cast<uint8_t>(buf[index]) == ParamMsg().cmd()) {
                if (auto res = ParamMsg::try_deserialize(
                            std::vector<char>(buf.begin() + static_cast<int64_t>(index + 1), buf.end())); res) {
                    msgs.push_back(std::unique_ptr<Message>(std::make_unique<ParamMsg>(res->first).release()));
                    index += res->second;
                }
            } else if (static_cast<uint8_t>(buf[index]) == BeginMsg().cmd()) {
                if (auto res = BeginMsg::try_deserialize(
                            std::vector<char>(buf.begin() + static_cast<int64_t>(index + 1), buf.end())); res) {
                    msgs.push_back(std::unique_ptr<Message>(std::make_unique<BeginMsg>(res->first).release()));
                    index += res->second;
                }
            } else if (static_cast<uint8_t>(buf[index]) == ShotMsg().cmd()) {
                if (auto res = ShotMsg::try_deserialize(
                            std::vector<char>(buf.begin() + static_cast<int64_t>(index + 1), buf.end())); res) {
                    msgs.push_back(std::unique_ptr<Message>(std::make_unique<ShotMsg>(res->first).release()));
                    index += res->second;
                }
            } else if (static_cast<uint8_t>(buf[index]) == StatusMsg().cmd()) {
                if (auto res = StatusMsg::try_deserialize(
                            std::vector<char>(buf.begin() + static_cast<int64_t>(index + 1), buf.end())); res) {
                    msgs.push_back(std::unique_ptr<Message>(std::make_unique<StatusMsg>(res->first).release()));
                    index += res->second;
                }
            } else if (static_cast<uint8_t>(buf[index]) == ResultMsg().cmd()) {
                if (auto res = ResultMsg::try_deserialize(
                            std::vector<char>(buf.begin() + static_cast<int64_t>(index + 1), buf.end())); res) {
                    msgs.push_back(std::unique_ptr<Message>(std::make_unique<ResultMsg>(res->first).release()));
                    index += res->second;
                }
            } else if (static_cast<uint8_t>(buf[index]) == IdMsg().cmd()) {
                if (auto res = IdMsg::try_deserialize(
                            std::vector<char>(buf.begin() + static_cast<int64_t>(index + 1), buf.end())); res) {
                    msgs.push_back(std::unique_ptr<Message>(std::make_unique<IdMsg>(res->first).release()));
                    index += res->second;
                }
            } else if (enable_extensions) {
                if (static_cast<uint8_t>(buf[index]) == ConnectMsg().cmd()) {
                    if (auto res = ConnectMsg::try_deserialize(
                                std::vector<char>(buf.begin() + static_cast<int64_t>(index + 1), buf.end())); res) {
                        msgs.push_back(std::unique_ptr<Message>(std::make_unique<ConnectMsg>(res->first).release()));
                        index += res->second;
                    }
                } else if (static_cast<uint8_t>(buf[index]) == ConfirmMsg().cmd()) {
                    if (auto res = ConfirmMsg::try_deserialize(
                                std::vector<char>(buf.begin() + static_cast<int64_t>(index + 1), buf.end())); res) {
                        msgs.push_back(std::unique_ptr<Message>(std::make_unique<ConfirmMsg>(res->first).release()));
                        index += res->second;
                    }
                }
            }

            index++;
        }

        return msgs;
    }

    QDataStream &operator<<(QDataStream &stream, const Message &msg) {
        std::vector<char> buf = msg.out();
        stream.writeRawData(buf.data(), buf.size());
        return stream;
    }
}