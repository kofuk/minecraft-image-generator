// SPDX-License-Identifier: MIT

#ifndef CONTAINERS_HH
#define CONTAINERS_HH

#include <filesystem>
#include <thread>

#include "logger/logger.hh"
#include "nbt/chunk.hh"
#include "nbt/region.hh"
#include "utils/path_hack.hh"

namespace pixel_terrain::image {
    class options {
        std::filesystem::path out_path_;
        bool out_path_is_dir_;
        unsigned int n_jobs_;
        bool is_nether_;
        std::string label_;
        std::filesystem::path cache_dir_;
        std::string outname_format_;

    public:
        options() { clear(); }

        void clear() {
            out_path_ = PATH_STR_LITERAL(".");
            n_jobs_ = std::thread::hardware_concurrency();
            is_nether_ = false;
            cache_dir_.clear();
            outname_format_.clear();
        }

        void set_out_path(std::filesystem::path const &p) {
            if (!std::filesystem::exists(p)) {
                out_path_is_dir_ = false;
            } else {
                out_path_is_dir_ = std::filesystem::is_directory(p);
            }

            out_path_ = p;
        }

        [[nodiscard]] auto out_path() const -> std::filesystem::path const & {
            return out_path_;
        }

        [[nodiscard]] auto out_path_is_directory() const -> bool {
            return out_path_is_dir_;
        }

        void set_n_jobs(unsigned int n) {
            if (n <= 0) {
                ILOG("Ignoring worker count because it is equal to or less "
                     "than 0.\n");
                return;
            }
            n_jobs_ = n;
        }

        [[nodiscard]] auto n_jobs() const -> unsigned int { return n_jobs_; }

        void set_is_nether(bool is_nether) { is_nether_ = is_nether; }

        [[nodiscard]] auto is_nether() const -> bool { return is_nether_; }

        void set_label(std::string const &label) { label_ = label; }

        [[nodiscard]] auto label() const -> std::string const & {
            return label_;
        }

        void set_cache_dir(std::filesystem::path const &path) {
            cache_dir_ = path;
        }

        [[nodiscard]] auto cache_dir() const -> std::filesystem::path const & {
            return cache_dir_;
        }

        void set_outname_format(std::string const &fmt) {
            outname_format_ = fmt;
        }

        [[nodiscard]] auto outname_format() const -> std::string const & {
            return outname_format_;
        }
    };

    class region_container {
        anvil::region *region_;
        options options_;
        std::filesystem::path out_file_;

    public:
        region_container(anvil::region *region, options options,
                         std::filesystem::path out_file)
            : region_(region), options_(std::move(options)),
              out_file_(std::move(out_file)) {}
        ~region_container() { delete region_; }

        [[nodiscard]] auto get_region() -> anvil::region * { return region_; }

        [[nodiscard]] auto get_output_path() const
            -> std::filesystem::path const * {
            return &out_file_;
        }

        [[nodiscard]] auto get_options() const -> options const * {
            return &options_;
        }
    };
} // namespace pixel_terrain::image

#endif
