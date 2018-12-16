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

#include <limits>
#include <core/semaphore.hh>

namespace ultramarine {

    namespace impl {
        struct local_actor {
        };
    }

    template<typename Derived, std::size_t ConcurrencyLimit = std::numeric_limits<std::size_t>::max()>
    struct local_actor : impl::local_actor {
        static_assert(ConcurrencyLimit > 0, "Local actor concurrency limit must be a positive integer");
        static constexpr std::size_t max_activations = ConcurrencyLimit;

        static thread_local std::size_t round_robin_counter;
    };
    template<typename Derived, std::size_t ConcurrencyLimit>
    thread_local std::size_t local_actor<Derived, ConcurrencyLimit>::round_robin_counter = 0;

    template <typename Derived>
    struct non_reentrant_actor {
        seastar::semaphore semaphore = seastar::semaphore(1);
    };
}
