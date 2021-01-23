// SPDX-License-Identifier: MIT

#include <cctype>

#include "nbt-path.hh"

namespace pixel_terrain::nbt {
    /*
     * Grammar
     *   pathspec = path-component*
     *   path-component = tag-compound-component | index
     *   tag-compound-component = path-separator tag-name
     *   path-separator = "/"
     *   tag-name = <escaped char>*
     *   index = "[" container-specifier? index-number "]"
     *   index-number = [1-9][0-9]*
     *   container-specifier = array-container-specifier
     *       | list-container-specifier
     *   array-container-specifier = "A"
     *   list-container-specifier = "L"
     */

    namespace {
        using iterator = std::string::const_iterator;

        auto parse_tag_name(iterator first, iterator last, std::string *out)
            -> iterator {
            bool verbatim = false;
            for (; first != last; ++first) {
                if (verbatim) {
                    out->push_back(*first);
                    verbatim = false;
                } else {
                    if (*first == '\\') {
                        verbatim = true;
                    } else if (*first == '/' || *first == '[') {
                        break;
                    } else {
                        out->push_back(*first);
                    }
                }
            }

            if (verbatim) {
                out->push_back('\\');
            }

            return first;
        }

        auto parse_path_separator(iterator first, iterator last, bool *ok)
            -> iterator {
            if (first == last) {
                *ok = false;
                return first;
            }

            if (*first == '/') {
                *ok = true;
                return first + 1;
            }
            *ok = false;
            return first;
        }

        auto parse_array_container_specifier(iterator first, iterator last,
                                             bool *ok) -> iterator {
            if (first == last) {
                *ok = false;
                return first;
            }

            if (*first == 'A') {
                *ok = true;
                return first + 1;
            }
            *ok = false;
            return first;
        }

        auto parse_list_container_specifier(iterator first, iterator last,
                                            bool *ok) -> iterator {
            if (first == last) {
                *ok = false;
                return first;
            }

            if (*first == 'L') {
                *ok = true;
                return first + 1;
            }
            *ok = false;
            return first;
        }

        auto parse_index_number(iterator first, iterator last, std::size_t *out,
                                bool *ok) -> iterator {
            if (first == last) {
                *ok = false;
                return first;
            }

            *ok = true;
            iterator itr = first;

            *out = 0;
            if (*itr < '1' || '9' < *itr) {
                *ok = false;
                return first;
            }
            *out = *itr - '0';
            ++itr;

            for (; itr != last; ++itr) {
                if (!static_cast<bool>(std::isdigit(*itr))) {
                    break;
                }
                *out *= 10;
                *out += *itr - '0';
            }

            return itr;
        }

        auto
        parse_container_specifier(iterator first, iterator last,
                                  enum nbt_path::pathspec::container *container)
            -> iterator {
            bool ok;
            iterator itr = parse_array_container_specifier(first, last, &ok);
            if (ok) {
                *container = nbt_path::pathspec::container::ARRAY;
                return itr;
            }

            itr = parse_list_container_specifier(itr, last, &ok);
            if (ok) {
                *container = nbt_path::pathspec::container::LIST;
                return itr;
            }

            *container = nbt_path::pathspec::container::UNSPEC;

            return itr;
        }

        auto parse_tag_compound_component(iterator first, iterator last,
                                          nbt_path::pathspec *spec, bool *ok)
            -> iterator {
            iterator itr = parse_path_separator(first, last, ok);
            if (!*ok) {
                return first;
            }

            std::string key;
            itr = parse_tag_name(itr, last, &key);

            *spec = nbt_path::pathspec(key);

            return itr;
        }

        auto parse_index(iterator first, iterator last,
                         nbt_path::pathspec *spec, bool *ok) -> iterator {
            *ok = true;

            if (first == last) {
                *ok = false;
                return first;
            }

            iterator itr = first;

            if (*itr != '[') {
                *ok = false;
                return first;
            }
            ++itr;

            enum nbt_path::pathspec::container cont;
            itr = parse_container_specifier(itr, last, &cont);

            std::size_t idx;
            itr = parse_index_number(itr, last, &idx, ok);
            if (!*ok) {
                return first;
            }

            if (itr == last || *itr != ']') {
                *ok = false;
                return first;
            }
            ++itr;

            *spec = nbt_path::pathspec(idx, cont);

            return itr;
        }

        auto parse_path_compoent(iterator first, iterator last,
                                 nbt_path::pathspec *spec, bool *ok)
            -> iterator {
            iterator itr = parse_tag_compound_component(first, last, spec, ok);
            if (*ok) {
                return itr;
            }

            itr = parse_index(itr, last, spec, ok);

            return itr;
        }
    } // namespace

    auto nbt_path::compile(std::string const &path) -> nbt_path * {
        auto *result = new nbt_path;

        iterator itr = path.begin();
        iterator last = path.end();

        pathspec spec;
        bool ok;
        do {
            itr = parse_path_compoent(itr, last, &spec, &ok);
            if (ok) {
                result->path_.push_back(spec);
            }
        } while (ok);

        if (itr != last) {
            delete result;
            return nullptr;
        }

        return result;
    }
} // namespace pixel_terrain::nbt
