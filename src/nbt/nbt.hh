// SPDX-License-Identifier: MIT

#ifndef NBT_HH
#define NBT_HH

#include <cstdint>
#include <vector>

#include "nbt/nbt-path.hh"
#include "nbt/tag.hh"

namespace pixel_terrain::nbt {
    class nbt {
        tag_compound *root_ = nullptr;

        auto parse_buffer(std::vector<std::uint8_t>::const_iterator first,
                          std::vector<std::uint8_t>::const_iterator last)
            -> bool;

        nbt() = default;

    public:
        ~nbt() { delete root_; }

        static auto
        from_iterator(std::vector<std::uint8_t>::const_iterator first,
                      std::vector<std::uint8_t>::const_iterator last) -> nbt *;

        template <class Tp, class = std::is_convertible<Tp, tag_payload>>
        auto query(nbt_path const &path) -> std::optional<Tp> {
            if (root_ == nullptr) {
                return std::nullopt;
            }

            nbt_path instance = path;
            tag_payload *result = root_->query(&instance);

            if (result == nullptr) {
                return std::nullopt;
            }

            if (Tp::type() == result->type()) {
                return *static_cast<Tp *>(result);
            } else {
                return std::nullopt;
            }
        }
    };
} // namespace pixel_terrain::nbt

#endif
