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

#include "core/distributed.hh"
#include "core/future.hh"

#include "actor.hpp"

namespace ultramarine {
    template<typename Actor>
    actor_ref<Actor> get_actor(actor_id id) {
        return actor_ref<Actor>(id);
    }

    class silo {
    public:
        seastar::future<> stop() {
            return seastar::make_ready_future();
        }
    };

    class silo_server {
    private:
        std::unique_ptr<seastar::distributed<silo>> _service;
    public:
        silo_server() : _service(new seastar::distributed<silo>) { }

        seastar::future<> start() {
            return _service->start();
        }

        seastar::future<> stop() {
            return _service->stop();
        }
    };
}