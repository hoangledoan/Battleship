#pragma once

#include <cstdint>
#include <optional>
#include <concepts>
#include <utility>

namespace kbm {
    template<typename DEST>
    concept CanTryParse = requires(DEST dest) {
        { DEST::try_parse(std::vector<char>().begin(), (int64_t) 0) } -> std::convertible_to<std::optional<std::pair<DEST, int64_t>>>;
    };
}