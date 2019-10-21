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
    template<typename Data>
    struct Animation {
        /// nullptr indicates the end of animation
        [[nodiscard]] virtual const Data *current() const noexcept = 0;

        /// nullptr indicates the end of animation
        virtual const Data *update(sf::Time elapsed) noexcept = 0;

        /// nullptr indicates the end of animation
        virtual const Data *reset() noexcept = 0;

        virtual ~Animation() = default;
    };

    template<typename Data>
    class BaseAnimation final : public Animation<Data> {
    public:
        BaseAnimation() = default;

        BaseAnimation(const BaseAnimation &) = default; // copy-constructible
        BaseAnimation &operator=(const BaseAnimation &) = default; // copy-assignable

        BaseAnimation(BaseAnimation &&) noexcept = default; // move-constructible
        BaseAnimation &operator=(BaseAnimation &&) noexcept = default; // move-assignable

        void addStep(const Data &&data, const sf::Time timer) noexcept {
            mData.push_back(std::move(data));
            mTimers.push_back(timer);
        }

        [[nodiscard]] inline const Data *current() const noexcept final {
            return mCurrentIndex < mData.size() ? &mData[mCurrentIndex] : nullptr;
        }

        const Data *update(const sf::Time elapsed) noexcept final {
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

        const Data *reset() noexcept final {
            mElapsed = sf::Time::Zero;
            mCurrentIndex = 0;
            return current();
        }

    private:
        std::vector<Data> mData;
        std::vector<sf::Time> mTimers;
        std::size_t mCurrentIndex{0};
        sf::Time mElapsed{sf::Time::Zero};
    };

    template<typename Data, typename Delegate = BaseAnimation<Data>>
    class DelegateAnimation : public Animation<Data> {
        static_assert(std::is_base_of<Animation<Data>, Delegate>::value);

    public:
        DelegateAnimation() = default;

        explicit DelegateAnimation(Delegate &&delegate) noexcept : mDelegate(std::move(delegate)) {};

        DelegateAnimation(const DelegateAnimation &) = default; // copy-constructible
        DelegateAnimation &operator=(const DelegateAnimation &) = default; // copy-assignable

        DelegateAnimation(DelegateAnimation &&) noexcept = default; // move-constructible
        DelegateAnimation &operator=(DelegateAnimation &&) noexcept = default; // move-assignable

        [[nodiscard]] inline const Data *current() const noexcept final {
            return mDelegate.current();
        }

        inline const Data *update(const sf::Time elapsed) noexcept final {
            return mDelegate.update(elapsed);
        }

        inline const Data *reset() noexcept final {
            return mDelegate.reset();
        }

    protected:
        Delegate mDelegate;
    };

    struct BlinkingText final : public DelegateAnimation<sf::Color> {
        BlinkingText();
    };

    using SpriteAnimation = DelegateAnimation<sf::IntRect>;
}
