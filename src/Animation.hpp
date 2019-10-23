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

namespace nongravitar::animation {
    template<typename T>
    struct Animation {
        /// nullptr indicates the end of animation
        [[nodiscard]] virtual const T *current() const noexcept = 0;

        /// nullptr indicates the end of animation
        virtual const T *update(sf::Time elapsed) noexcept = 0;

        /// nullptr indicates the end of animation
        virtual const T *reset() noexcept = 0;

        virtual ~Animation() = default;
    };

    template<typename T>
    class BaseAnimation : public Animation<T> {
    public:
        BaseAnimation(const BaseAnimation &) = default; // copy-constructible
        BaseAnimation &operator=(const BaseAnimation &) = default; // copy-assignable

        BaseAnimation(BaseAnimation &&) noexcept = default; // move-constructible
        BaseAnimation &operator=(BaseAnimation &&) noexcept = default; // move-assignable

        [[nodiscard]] inline const T *current() const noexcept final {
            return mCurrentIndex < mData.size() ? &mData[mCurrentIndex] : nullptr;
        }

        const T *update(const sf::Time elapsed) noexcept final {
            if (mCurrentIndex < mTimers.size()) {
                mElapsed += elapsed;

                if (const auto &timer = mTimers[mCurrentIndex]; mElapsed >= timer) {
                    mElapsed -= timer;

                    if (++mCurrentIndex < mData.size()) {
                        return &mData[mCurrentIndex];
                    }
                } else {
                    return &mData[mCurrentIndex];
                }
            }

            return nullptr;
        }

        const T *reset() noexcept final {
            mElapsed = sf::Time::Zero;
            mCurrentIndex = 0;
            return current();
        }

    protected:
        BaseAnimation() = default;

        void addStep(const T &&data, const sf::Time timer) noexcept {
            mData.push_back(std::move(data));
            mTimers.push_back(timer);
        }

    private:
        std::vector<T> mData;
        std::vector<sf::Time> mTimers;
        std::size_t mCurrentIndex{0};
        sf::Time mElapsed{sf::Time::Zero};
    };

    struct BlinkingText final : public BaseAnimation<sf::Color> {
        BlinkingText();
    };
}
