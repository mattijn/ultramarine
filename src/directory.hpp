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

namespace ultramarine {
    template<typename Actor>
    struct actor_directory {

        template<typename KeyType>
        [[nodiscard]] static constexpr auto hash_key(KeyType &&key) noexcept {
            return std::hash<ActorKey<Actor>>{}(key);
        }

        [[nodiscard]] static constexpr Actor *hold_activation(ActorKey<Actor> const& key, actor_id id) {
            if (!Actor::directory) [[unlikely]] {
                Actor::directory = std::make_unique<ultramarine::directory<Actor>>();
            }

            auto r = std::get<0>(Actor::directory->try_emplace(id, key));
            return &(std::get<1>(*r));
        }
    };
}