/*
 * Copyright (c) 2020 Koki Fukuda
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Logger, that can be used from different thread safely. */

#include <iostream>
#include <mutex>

#include "logger.hh"

namespace pixel_terrain::logger {
    namespace {
        std::mutex m;

        std::size_t generated;
        std::size_t reused;
    }

    void d(std::string message) {
        std::unique_lock<std::mutex> lock(m);
        std::cerr << message << '\n';
    }

    void e(std::string message) {
        std::unique_lock<std::mutex> lock(m);
        std::cerr << message << '\n';
    }

    void i(std::string message) {
        std::unique_lock<std::mutex> lock(m);
        std::cout << message << '\n';
    }

    void record_stat(bool regenerated) {
        std::unique_lock<std::mutex> lock(m);

        if (regenerated) ++generated;
        else ++reused;
    }

    void show_stat() {
        std::cout << "Statistics:\n";
        std::cout << "  Chunks generated: " << generated << '\n';
        std::cout << "  Chunks reused:    " << reused << '\n';
        std::cout << "  % reused:         " << (reused * 100) / (generated + reused) << '\n';
    }
} // namespace pixel_terrain::logger
