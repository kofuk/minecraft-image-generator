// SPDX-License-Identifier: MIT

#include <array>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <ostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef TEST
#include <boost/test/tools/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#endif

/*
 * DATA STRUCTURE
 *
 * |-------------------|-------------------------|
 * | block_id.size()   | length of block_id      |
 * | block_id (string) | block id (byte by byte) |
 * | .                 |                         |
 * | .                 |                         |
 * | .                 |                         |
 * | padding.size()    |                         |
 * | padding           |                         |
 * | ...               |                         |
 * | color[0]          |                         |
 * | color[1]          |                         |
 * | color[2]          |                         |
 * | color[3]          |                         |
 * |-------------------+-------------------------|
 * |                   |                         |
 * | .                 |                         |
 * | .                 |                         |
 * | .                 |                         |
 * |                   |                         |
 * |-------------------+-------------------------|
 * | 0                 | sentinel value          |
 * |-------------------+-------------------------|
 *
 * order of color 0--3 is byte-order-dependent.
 * this program puts the bytes to construct 32 bits of int
 * represents RRGGBBAA.
 */

namespace {
    size_t total_elements;
    size_t total_bytes;

    void print_usage() {
        std::cout << "usage: block_data_generator outfile infile...\n";
    }

    class block_color {
        /* Namespaced block ID */
        std::string namespace_id_;

        /* Color components associated with this block, [r, g, b, a] in order.
         */
        std::array<std::uint8_t, 4> color_;

    public:
        block_color() = default;

        [[nodiscard]] auto namespace_id() const -> std::string const & {
            return namespace_id_;
        }

        [[nodiscard]] auto color() -> std::array<std::uint8_t, 4> & {
            return color_;
        }

        [[nodiscard]] auto color() const
            -> std::array<std::uint8_t, 4> const & {
            return color_;
        }

        [[nodiscard]] auto required_padding() const -> unsigned int {
            unsigned int size = namespace_id_.size() + 2;
            if (size % 4 == 0) {
                return 0;
            }
            return 4 - size % 4;
        }

        [[nodiscard]] auto total_length() const -> unsigned int {
            return 2 + namespace_id_.size() + required_padding() + 4;
        }

        void set_namespace_id(std::string const &namespace_id) {
            namespace_id_ = namespace_id;
        }

        void set_color(std::array<std::uint8_t, 4> const &color) {
            color_ = color;
        }

        auto operator==(block_color const &another) const -> bool {
            return namespace_id_ == another.namespace_id_;
        }

        auto operator<(block_color const &another) const -> bool {
            return namespace_id_ < another.namespace_id_; // NOLINT
        }

        auto operator>(block_color const &another) const -> bool {
            return namespace_id_ > another.namespace_id_; // NOLINT
        }
    };

    void write_do_not_edit_mark(std::ostream &out_file) {
        out_file
            << "// \e[3D\e[43;1m                                               "
               "                            \e[0m\n"
               "// \e[3D\e[43m   \e[40;37;1m DO NOT EDIT!                      "
               "                                  \e[43m   \e[0m\n"
               "// \e[3D\e[43m   \e[40;37;1m This file has been automatically "
               "generated by block_data_generator. \e[43m   \e[0m\n"
               "// \e[3D\e[43;1m                                               "
               "                            \e[0m\n";
    }

    void write_preamble(std::ostream &out_file) {
        write_do_not_edit_mark(out_file);

        out_file << R"(
#ifdef EMBEDED_BLOCK_COLOR_DATA_H
#  error block color data has already embeded.
#else
#include <cstdint>
namespace {

// NOLINTNEXTLINE: This header file is auto generated and designed to only to embed data.
const std::uint8_t block_colors_data[] = {
)";
    }

    void write_postamble(std::ostream &out_file) {
        out_file << R"(0};
} /* namespace */

)";
        ++total_bytes;

        out_file << "/* " << total_bytes << " bytes, " << total_elements
                 << " entries written. */\n";

        out_file << "#endif\n";

        write_do_not_edit_mark(out_file);
    }

    auto split_field(std::string const &record) -> std::vector<std::string> {
        std::vector<std::string> result;
        size_t beg = 0;
        for (size_t i = 0; i < record.size(); ++i) {
            if (record[i] == '\t') {
                result.push_back(record.substr(beg, i - beg));
                beg = i + 1;
            }
        }
        result.push_back(record.substr(beg, record.size() - beg));
        return result;
    }

    auto load_colors(std::string const &in_name, std::set<block_color> *to)
        -> bool {
        std::ifstream in_file(in_name);
        if (!in_file) {
            std::cerr << "fatal: cannot open " << in_name << '\n';
            return false;
        }

        std::string line;
        if (!std::getline(in_file, line)) {
            std::cerr << "fatal: empty file: " << in_name << '\n';
            return false;
        }
        size_t lineno = 2;
        for (; std::getline(in_file, line); ++lineno) {
            block_color element;

            /* Skip comment line. */
            if (!line.empty() && line[0] == '#') {
                continue;
            }

            std::vector<std::string> fields = split_field(line);
            if (fields.size() != 5) { // NOLINT
                std::cerr << "fatal: invalid line: " << in_name << ": "
                          << lineno << '\n';
                return false;
            }
            element.set_namespace_id(fields[0]);
            if (std::numeric_limits<std::uint8_t>::max() <
                element.namespace_id().size()) {
                /* Namespace ID longer than 255 can't be saved because of
                   limitation of data structure. */
                std::cerr << "fatal: namespace id too long: " << in_name << ": "
                          << lineno << '\n';
                return false;
            }

            for (int c = 0; c < 4; ++c) {
                try {
                    int color = std::stoi(fields[c + 1], nullptr, 16); // NOLINT
                    if (color < 0 ||
                        std::numeric_limits<std::uint8_t>::max() < color) {
                        std::cerr
                            << "fatal: invalid color component (out of range): "
                            << in_name << ": " << lineno << '\n';
                        return false;
                    }
                    element.color()[c] = color;
                } catch (std::invalid_argument const &e) {
                    std::cerr << "fatal: invalid color component: " << in_name
                              << ": " << lineno << '\n';
                    return false;
                }
            }

            auto [_, inserted] = to->insert(element);
            if (!inserted) {
                std::cerr << "warning: duplicate block: "
                          << element.namespace_id() << '\n';
            }
        }

        return true;
    }

    auto write_content(std::set<block_color> const &elements,
                       std::ostream &out_file) -> bool {
        for (block_color const &element : elements) {
            auto block_id_len = (std::uint8_t)element.namespace_id().size();
            out_file << +block_id_len << ',';
            for (char const &chr : element.namespace_id()) {
                out_file << '\'' << chr << '\'';
                out_file << ',';
            }
            out_file << element.required_padding() << ',';
            for (unsigned int i = 0, len = element.required_padding(); i < len; ++i) {
                out_file << "0,";
            }
            total_bytes += element.total_length();

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            for (int i = 3; i >= 0; --i) {
                out_file << +element.color()[i] << ',';
            }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            for (int i = 0; i < 4; ++i) {
                out_file << +element.color[i] << ',';
            }
#else
#error "Unsupported byte order"
#endif

            out_file << '\n';
            total_bytes += 4;
            ++total_elements;
        }

        return true;
    }
} // namespace

#ifndef TEST
auto main(int argc, char **argv) -> int {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        print_usage();
        return 1;
    }

    std::set<block_color> elements;
    for (int i = 2; i < argc; ++i) {
        if (!load_colors(argv[i], &elements)) {
            std::cerr << "fatal: cannot load " << argv[i] << '\n';
            return 1;
        }
    }

    char *out_name = argv[1];
    std::ofstream out_file(out_name);
    if (!out_file) {
        std::cerr << "fatal: cannot open " << out_name << '\n';
        return 1;
    }

    write_preamble(out_file);

    if (!write_content(elements, out_file)) {
        std::cerr << "fatal: cannot write block data\n";
        return 1;
    }

    write_postamble(out_file);
}
#endif

#ifdef TEST
BOOST_AUTO_TEST_CASE(split_field_test) {
    std::vector<std::string> fields = split_field("foo\tbar\tbaz");
    BOOST_TEST(fields.size() == 3);
    BOOST_TEST(fields[0] == "foo");
    BOOST_TEST(fields[1] == "bar");
    BOOST_TEST(fields[2] == "baz");

    fields = split_field("\tfoo\tbar\tbaz\t");
    BOOST_TEST(fields.size() == 5);
    BOOST_TEST(fields[0] == "");
    BOOST_TEST(fields[1] == "foo");
    BOOST_TEST(fields[2] == "bar");
    BOOST_TEST(fields[3] == "baz");
    BOOST_TEST(fields[4] == "");
}
#endif
