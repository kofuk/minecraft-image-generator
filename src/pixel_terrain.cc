/*
 * Entry point for pixel-terrain.
 * Parse option and all call specific function.
 */

#include <algorithm>
#include <array>
#include <csetjmp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

#include "commands/generate/generate_main.hh"
#include "commands/server/server.hh"

namespace pixel_terrain {
    namespace {
        void print_usage() {
            std::cout << "Usage: mcmap generate [OPTION]... SRC_DIR"
                      << std::endl;
            std::cout << "       mcmap server [OPTION]..." << std::endl;
            std::cout << "       mcmap --version" << std::endl;
            std::cout
                << "For help for each mode, execute these mode with `--help' "
                   "option."
                << std::endl;
        }

        void print_version() {
            std::cout << "mcmap 3.0" << std::endl;
            std::cout << "Copyright (C) 2020, Koki Fukuda." << std::endl;
            std::cout
                << "This program includes C++ re-implementation of" << std::endl
                << "anvil-parser and nbt, originally written in Python."
                << std::endl
                << "Visit https://github.com/kofuk/minecraft-image-gemerator"
                << std::endl
                << "for more information and the source code." << std::endl;
        }

        void handle_commands(int argc, char **argv) {
            if (!std::strcmp(argv[0], "generate")) {
                commands::generate::main(argc, argv);
            } else if (!std::strcmp(argv[0], "server")) {
                commands::server::main(argc, argv);
            } else {
                std::cout << "unrecognized option: " << argv[0] << std::endl;
                print_usage();
                exit(1);
            }
        }
    } // namespace
} // namespace pixel_terrain

int main(int argc, char **argv) {
    if (argc < 2) {
        pixel_terrain::print_usage();
        std::exit(1);
    }

    if (!strcmp(argv[1], "--version")) {
        pixel_terrain::print_version();
        std::exit(0);
    } else if (argv[1][0] == '-' || argv[1][0] == '/') {
        pixel_terrain::print_usage();
        std::exit(0);
    } else {
        pixel_terrain::handle_commands(--argc, ++argv);
    }

    return 0;
}
