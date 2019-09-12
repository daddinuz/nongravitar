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

#include <functional>

#include <SFML/Graphics.hpp>

#include "wrapper.hpp"

namespace gravitar {
    template<typename D, typename F>
    class Animation final : public sf::Drawable, public Wrapper<D> {
    public:
        static_assert(std::is_base_of<sf::Drawable, D>::value);

        using Update = std::function<void(D &, F &)>;

        Animation() = delete; // no default-constructible

        Animation(unsigned fps, Update update, D &&drawable, F &&frames);

        Animation(const Animation &) = delete; // no copy-constructible
        Animation &operator=(const Animation &) = delete; // no copy-assignable

        Animation(Animation &&) = delete; // no move-constructible
        Animation &operator=(Animation &&) = delete; // no move-assignable

        void update(const sf::Time &time);

        void resetTimer(bool keepReminder = false);

        const sf::Time &elapse(const sf::Time &time);

        [[nodiscard]] const sf::Time &getTimer() const noexcept;

        void setFramePerSecond(unsigned value) noexcept;

        [[nodiscard]] unsigned getFramePerSecond() const;

    private:
        void draw(sf::RenderTarget &target, sf::RenderStates states) const final;

        F mFrames;
        Update mUpdate;
        sf::Time mTimer{sf::Time::Zero}, mFrameTime{sf::Time::Zero};
    };

    /*
     * Implementation
     */

    template<typename D, typename F>
    Animation<D, F>::Animation(unsigned fps, Animation::Update update, D &&drawable, F &&frames)
            : Wrapper<D>(std::move(drawable)), mFrames(std::move(frames)), mUpdate{update} { setFramePerSecond(fps); }

    template<typename D, typename F>
    void Animation<D, F>::update(const sf::Time &time) {
        if (elapse(time) >= mFrameTime) {
            mUpdate(this->get(), mFrames);
            resetTimer(true);
        }
    }

    template<typename D, typename F>
    void Animation<D, F>::resetTimer(bool keepReminder) {
        if (keepReminder) {
            mTimer = sf::microseconds(mTimer.asMicroseconds() % mFrameTime.asMicroseconds());
        } else {
            mTimer = sf::Time::Zero;
        }
    }

    template<typename D, typename F>
    const sf::Time &Animation<D, F>::elapse(const sf::Time &time) {
        return mTimer += time;
    }

    template<typename D, typename F>
    const sf::Time &Animation<D, F>::getTimer() const noexcept {
        return mTimer;
    }

    template<typename D, typename F>
    void Animation<D, F>::setFramePerSecond(unsigned value) noexcept {
        mFrameTime = sf::seconds(1.0f / ((value > 0u) ? static_cast<float>(value) : std::numeric_limits<float>::min()));
    }

    template<typename D, typename F>
    unsigned Animation<D, F>::getFramePerSecond() const {
        return sf::seconds(1.0f) / mFrameTime;
    }

    template<typename D, typename F>
    void Animation<D, F>::draw(sf::RenderTarget &target, sf::RenderStates states) const {
        (void) states;
        target.draw(this->get());
    }
}
