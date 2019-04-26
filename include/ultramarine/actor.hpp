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

#include <memory>
#include <unordered_map>
#include <boost/core/noncopyable.hpp>
#include <seastar/core/reactor.hh>
#include "directory.hpp"
#include "impl/directory.hpp"
#include "impl/macro.hpp"

namespace ultramarine {
    template<typename Derived, typename LocalPlacementStrategy = default_local_placement_strategy>
    struct actor : private boost::noncopyable {
        using KeyType = actor_id;
        using PlacementStrategy = LocalPlacementStrategy;

        static thread_local std::unique_ptr<impl::directory<Derived>> directory;

        static seastar::future<> clear_directory() {
            return seastar::smp::invoke_on_all([] {
                if (directory) {
                    directory->clear();
                }
                return seastar::make_ready_future();
            });
        }
    };

    template<typename Derived, typename LocalPlacementStrategy>
    thread_local std::unique_ptr<impl::directory<Derived>> actor<Derived, LocalPlacementStrategy>::directory;
}

#include "actor_traits.hpp"
