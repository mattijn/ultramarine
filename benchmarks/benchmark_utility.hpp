/*
 * MIT License
 *
 * Copyright (c) 2018 Hippolyte Barraud
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

#pragma once

#include <chrono>
#include <numeric>
#include <seastar/core/app-template.hh>
#include <seastar/core/print.hh>
#include <ultramarine/cluster/impl/service.hpp>

#define ULTRAMARINE_BENCH(name) {#name, name}

template<typename T>
T mean(const std::vector<T> &data) {
    return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}

template<typename T>
T variance(const std::vector<T> &data) {
    double xBar = mean(data);
    double sqSum = std::inner_product(data.begin(), data.end(), data.begin(), 0.0);
    return sqSum / data.size() - xBar * xBar;
}

template<typename T>
T stDev(const std::vector<T> &data) {
    return std::sqrt(variance(data));
}

namespace ultramarine::benchmark {

    using benchmark_list = std::initializer_list<std::pair<std::string_view, seastar::future<> (*)()>>;

    template<typename Pair>
    auto run_one(Pair &&bench, int run) {
        using namespace std::chrono;

        int *counter = new int(0);
        auto bench_start = high_resolution_clock::now();
        return seastar::do_with(std::move(bench), std::vector<microseconds::rep>(), [counter, run, bench_start]
                (auto &bench, auto &vec) {
            return seastar::do_until([counter, run] { return *counter >= run; }, [counter, &bench, &vec] {
                ++*counter;
                auto start = high_resolution_clock::now();
                return std::get<1>(bench)().then([start, &vec] {
                    auto stop = high_resolution_clock::now();
                    vec.emplace_back(duration_cast<microseconds>(stop - start).count());
                    return seastar::make_ready_future();
                });
            }).then([&vec, &bench, bench_start] {
                std::sort(std::begin(vec), std::end(vec));
                auto sum = std::accumulate(std::begin(vec), std::end(vec), 0UL);
                seastar::print("%s: %lu us (min: %lu us -- 99.9p: %lu us -- std: %lu us) | Total: %lu ms\n",
                               std::get<0>(bench), sum / vec.size(),
                               *std::begin(vec), *(std::end(vec) - 1), stDev(vec),
                               duration_cast<milliseconds>(high_resolution_clock::now() - bench_start).count());
            });
        });
    }

    int run(int ac, char **av, benchmark_list &&benchs, int run = 1000) {
        seastar::app_template app;

        return app.run(ac, av, [benchs = std::move(benchs), run] {
            return seastar::do_with(std::move(benchs), std::vector<std::chrono::microseconds::rep>(), [run]
                    (auto &benchs, auto &vec) {
                return seastar::do_for_each(benchs, [run](auto &bench) {
                    return run_one(bench, run);
                });
            });
        });
    }
}