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

#include "benchmark_utility.hpp"
#include "actor.hpp"
#include "actor_ref.hpp"
#include "macro.hpp"

class counter_actor : ultramarine::actor {
public:
    volatile int counter = 0;

    seastar::future<> increase_counter() {
        counter++;
        return seastar::make_ready_future();
    }

    ULTRAMARINE_DEFINE_ACTOR(counter_actor, (increase_counter));
};
ULTRAMARINE_IMPLEMENT_ACTOR(counter_actor);

auto plain_object() {
    auto counterActor = new counter_actor(0);
    int *counter = new int(0);

    return seastar::do_until([counterActor, counter] {
        return *counter > 10000;
    }, [counterActor, counter] {
        ++*counter;
        return counterActor->increase_counter();
    });
}

auto local_actor() {
    int *counter = new int(0);

    return seastar::do_with(ultramarine::get<counter_actor>(0), [counter](auto &counterActor) {
        return seastar::do_until([counter] {
            return *counter > 10000;
        }, [&counterActor, counter] {
            ++*counter;
            return counterActor.tell(counter_actor::message::increase_counter());
        });
    });
}

auto collocated_actor() {
    int *counter = new int(0);

    return seastar::do_with(ultramarine::get<counter_actor>(1), [counter](auto &counterActor) {
        return seastar::do_until([counter] {
            return *counter > 10000;
        }, [&counterActor, counter] {
            ++*counter;
            return counterActor.tell(counter_actor::message::increase_counter());
        });
    });
}

int main(int ac, char **av) {
    return ultramarine::benchmark::run(ac, av, {ULTRAMARINE_BENCH(plain_object), ULTRAMARINE_BENCH(local_actor),
                                                ULTRAMARINE_BENCH(collocated_actor)});
}