// SPDX-License-Identifier: MIT

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include <regetopt.h>

#include "config.h"
#include "image/image.hh"
#include "logger/logger.hh"
#include "nbt/utils.hh"
#include "pixel-terrain.hh"
#include "utils/array.hh"
#include "utils/path_hack.hh"

namespace {
    pixel_terrain::image::image_generator *generator;

    void generate_image(std::string const &src,
                        pixel_terrain::image::options options) {
        using namespace pixel_terrain;

        if (options.label().empty()) {
            options.set_label(src);
        }

        if (generator == nullptr) {
            generator = new image::image_generator(options);
            generator->start();
        }

        std::filesystem::path src_path(src);
        if (std::filesystem::is_directory(src_path)) {
            generator->queue_all_in_dir(src, options);
        } else {
            generator->queue_region(src, options);
        }
    }

    void clean_up_generator() {
        using namespace pixel_terrain;

        if (generator != nullptr) {
            DLOG("Waiting for worker to finish...\n");
            generator->finish();

            delete generator;

            logger::show_stat();
        }
    }
} // namespace

namespace {
    void print_usage() {
        std::cout << &R"(
Usage: pixel-terrain image [option]... [--] <dir>    OR
       pixel-terrain image [option]... [--] <input file>    OR
       pixel-terrain image [option]... --generate=<src1> [[--clear] [option]... --generate=<src2>]...

Generate map image.

  -c DIR, --cache-dir=DIR   Use DIR as cache direcotry.
      --clear               Reset current generator configuration.
      --generate=SRC        Generate image for SRC with current configuration.
  -j N, --jobs=N            Execute N jobs concurrently. Take effects only if
                            specified before --generate option specified.
      --label               Label current configuration.
  -n, --nether              Use image generator optimized to nether.
  -o PATH, --out=PATH       Save generated images to PATH.
                            If PATH is a file, write output image to PATH eve if
                            there are multiple input file. Otherwise, output
                            filename is decided by format of --outname-format or
                            input filename.
      --outname-format=FMT  Specify format for output filename. Default value is
                            original filename with extension appended. Note that
                            proper extension will be appended automatically.
  -V, -VV, -VVV             Set log level. Specifying multiple times increases log level.
                            Note that --clear option does NOT clear this value.
      --help                Print this usage and exit.

Output name format speficier:
 %X    X-coordinate of the region.
 %Z    Z-coordinate of the region.
 %%    A '%' character.
 )"[1];
    }

    auto long_options = pixel_terrain::make_array<::re_option>(
        ::re_option{"jobs", re_required_argument, nullptr, 'j'},
        ::re_option{"cache-dir", re_required_argument, nullptr, 'c'},
        ::re_option{"clear", re_no_argument, nullptr, 'C'},
        ::re_option{"generate", re_required_argument, nullptr, 'G'},
        ::re_option{"nether", re_no_argument, nullptr, 'n'},
        ::re_option{"out", re_required_argument, nullptr, 'o'},
        ::re_option{"outname-format", re_required_argument, nullptr, 'F'},
        ::re_option{"label", re_required_argument, nullptr, 'l'},
        ::re_option{"help", re_no_argument, nullptr, 'h'},
        ::re_option{nullptr, 0, nullptr, 0});
} // namespace

namespace pixel_terrain {
    auto image_main(int argc, char **argv) -> int {
        std::atexit(&clean_up_generator);

        pixel_terrain::image::options options;

        bool should_generate = true;

        for (;;) {
            int opt = regetopt(argc, argv, "j:c:no:F:V", long_options.data(),
                               nullptr);
            if (opt < 0) {
                break;
            }

            should_generate = true;

            switch (opt) {
            case 'V':
                ++pixel_terrain::logger::log_level;
                break;

            case 'j':
                try {
                    options.set_n_jobs(std::stoi(::re_optarg));
                } catch (std::invalid_argument const &) {
                    std::cout << "Invalid concurrency.\n";
                    std::exit(1);
                } catch (std::out_of_range const &) {
                    std::cout << "Concurrency is out of permitted range.\n";
                    std::exit(1);
                }
                break;

            case 'n':
                options.set_is_nether(true);
                break;

            case 'o':
                options.set_out_path(::re_optarg);
                break;

            case 'F':
                options.set_outname_format(::re_optarg);
                break;

            case 'c':
                options.set_cache_dir(::re_optarg);
                break;

            case 'C':
                options.clear();
                break;

            case 'G':
                generate_image(::re_optarg, options);
                should_generate = false;
                break;

            case 'l':
                options.set_label(::re_optarg);
                break;

            case 'h':
                print_usage();
                std::exit(0);

            default:
                return 1;
            }
        }

        if (should_generate) {
            if (::re_optind == argc) {
                print_usage();
                std::exit(1);
            }

            for (int i = ::re_optind; i < argc; ++i) {
                generate_image(argv[i], options);
            }
        }

        return 0;
    }
} // namespace pixel_terrain
