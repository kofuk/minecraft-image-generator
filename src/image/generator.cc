// SPDX-License-Identifier: MIT

/* Helper to use multi-thread efficiently. */

#include <algorithm>
#include <condition_variable>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iterator>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "image/image.hh"
#include "image/utils.hh"
#include "image/worker.hh"
#include "logger/logger.hh"
#include "nbt/region.hh"
#include "nbt/utils.hh"
#include "utils/path_hack.hh"
#include "utils/threaded_worker.hh"

namespace pixel_terrain::image {
    namespace {
        auto make_output_name(std::filesystem::path const &input,
                              options const &options)
            -> std::pair<std::filesystem::path, bool> {
            std::filesystem::path out_file;
            if (options.outname_format().empty()) {
                auto [out, ok] =
                    make_output_name_by_input(input, options.out_path());
                if (!ok) {
                    ELOG("Cannot decide output name for %s.\n",
                         input.filename().string().c_str());
                    return std::make_pair("", false);
                }

                out_file = out;
            } else {
                auto [rx, rz, ok] = parse_region_file_path(input);

                if (!ok) {
                    ILOG("Filename %s is not r.M.N.mca form; it'll be "
                         "processed, but --outname-format won't work "
                         "properly.\n",
                         input.filename().string().c_str());

                    auto [name, ok] =
                        make_output_name_by_input(input, options.out_path());
                    if (!ok) {
                        ELOG("Cannot decide output name for %s.\n",
                             input.filename().string().c_str());
                        return std::make_pair("", false);
                    }
                } else {
                    DLOG("Formatting filename for %s.\n",
                         input.string().c_str());
                    path_string out_name =
                        format_output_name(options.outname_format(), rx, rz) +
                        PATH_STR_LITERAL(".png");

                    out_file = options.out_path() / out_name;
                }
            }

            return std::make_pair(out_file, true);
        }
    } // namespace

    void image_generator::queue(region_container *item) {
        DLOG("Queue: %s\n",
             item->get_output_path()->filename().string().c_str());

        thread_pool_->queue_job(item);
    }

    void image_generator::queue_region(std::filesystem::path const &region_file,
                                       options const &options) {
        DLOG("Preparing %s for queuing...\n",
             region_file.filename().string().c_str());

        if (region_file.extension().string() != ".mca") {
            ILOG("Skipping %s because it is not a .mca file.\n",
                 region_file.filename().string().c_str());
            return;
        }

        anvil::region *r;
        try {
            if (options.cache_dir().empty()) {
                r = new anvil::region(region_file);
            } else {
                r = new anvil::region(region_file, options.cache_dir());
            }
        } catch (std::exception const &e) {
            ELOG("Failed to read region: %s\n", region_file.string().c_str());
            ELOG("%s\n", e.what());

            return;
        }

        std::filesystem::path out_file;
        if (options.out_path_is_directory()) {
            auto [out, ok] = make_output_name(region_file, options);
            if (!ok) {
                ILOG("Cannot decide output name for %s.\n",
                     region_file.filename().string().c_str());
            }
            out_file = out;
        } else {
            DLOG("Output path for %s (%s) is not directory.\n",
                 region_file.filename().string().c_str(),
                 options.out_path().string().c_str());
            out_file = options.out_path();
        }

        DLOG("Output filename is %s.\n", out_file.string().c_str());

        queue(new region_container(r, options, out_file));
        logger::progress_bar_increase_total(1);
    }

    void image_generator::queue_all_in_dir(std::filesystem::path const &dir,
                                           options const &options) {
        if (!options.out_path_is_directory()) {
            ILOG("Output path pointing a single file; this "
                 "may cause unexpected result.\n");
        }

        if (!options.cache_dir().empty()) {
            try {
                std::filesystem::create_directories(options.cache_dir());
            } catch (std::filesystem::filesystem_error const &e) {
                ELOG("Cannot create cache directory for %s: %s\n",
                     options.label().c_str(), e.what());
            }
        }

        for (std::filesystem::directory_entry const &path :
             std::filesystem::directory_iterator(dir)) {
            if (path.is_directory()) {
                continue;
            }

            queue_region(path.path(), options);
        }
    }

    void image_generator::start() {
        DLOG("Starting worker thread(s) ...\n");

        thread_pool_->start();
    }

    void image_generator::finish() { thread_pool_->finish(); }
} // namespace pixel_terrain::image
