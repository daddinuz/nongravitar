/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Davide Di Carlo
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

namespace nongravitar {
    template<typename T>
    class Animation {
    public:
        Animation() = default;

        Animation(const Animation &) = delete; // no copy-constructible
        Animation &operator=(const Animation &) = delete; // no copy-assignable

        Animation(Animation &&) = delete; // no move-constructible
        Animation &operator=(Animation &&) = delete; // no move-assignable

        void addFrame(const T &&data, const sf::Time timer) noexcept {
            mData.push_back(std::move(data));
            mTimers.push_back(timer);
        }

        /// nullptr indicates the end of animation
        inline const T *current() const noexcept {
            return mCurrentFrameIndex < mData.size() ? &mData[mCurrentFrameIndex] : nullptr;
        }

        /// nullptr indicates the end of animation
        const T *reset() noexcept {
            mElapsed = sf::Time::Zero;
            mCurrentFrameIndex = 0;
            return current();
        }

        /// nullptr indicates the end of animation
        const T *update(const sf::Time elapsed) noexcept {
            if (mCurrentFrameIndex < mTimers.size()) {
                mElapsed += elapsed;

                if (const auto &timer = mTimers[mCurrentFrameIndex]; mElapsed >= timer) {
                    mElapsed -= timer;

                    if (++mCurrentFrameIndex < mData.size()) {
                        return &mData[mCurrentFrameIndex];
                    }
                } else {
                    return &mData[mCurrentFrameIndex];
                }
            }

            return nullptr;
        }

    private:
        std::vector<T> mData;
        std::vector<sf::Time> mTimers;
        std::size_t mCurrentFrameIndex{0};
        sf::Time mElapsed{sf::Time::Zero};
    };
}
