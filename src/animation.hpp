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
#include "spritesheet.hpp"

namespace gravitar {
    class Animation : virtual public sf::Drawable {
    public:
        virtual void update(const sf::Time &time) = 0;

        void resetTimer(bool keepReminder = false);

        const sf::Time &elapse(const sf::Time &time);

        [[nodiscard]] const sf::Time &getTimer() const;

        void setFrameTime(sf::Time time);

        [[nodiscard]] const sf::Time &getFrameTime() const;

        void setLoop(bool loop);

        [[nodiscard]] bool getLoop() const;;

    private:
        sf::Time mTimer, mFrameTime;
        bool mLoop{true};
    };

    class SpriteAnimation final : public Animation, public SpriteSheet {
    public:
        SpriteAnimation() = delete;

        const SpriteAnimation &operator=(const SpriteAnimation &) = delete;

        explicit SpriteAnimation(const sf::Texture &texture);

        static SpriteAnimation from(const sf::Texture &texture, sf::Vector2i startCoord, sf::Vector2i table, sf::Vector2i frame, sf::Time frameTime = sf::Time::Zero);

        void update(const sf::Time &time) final;

    private:
        SpriteAnimation(const SpriteAnimation &) = default;

        SpriteAnimation(const sf::Texture &texture, sf::Vector2i startCoord, sf::Vector2i table, sf::Vector2i frame, sf::Time frameTime);
    };

    template<typename T, typename F>
    class DelegateAnimation final : public Animation {
    public:
        static_assert(std::is_base_of_v<sf::Drawable, T>);

        DelegateAnimation(std::function<void(T &, const F &)> update, T t, F f, sf::Time frameTime = sf::Time::Zero)
                : mUpdate(update), mF(f), mT(t) {
            setFrameTime(frameTime);
        }

        void update(const sf::Time &time) final;

        T &operator*();

        const T &operator*() const;

        T *operator->();

        const T *operator->() const;

    private:
        void draw(sf::RenderTarget &target, sf::RenderStates states) const final;

        std::function<void(T &, const F &)> mUpdate;
        F mF;
        T mT;
    };

    template<typename T, typename F>
    void DelegateAnimation<T, F>::update(const sf::Time &time) {
        if (elapse(time) >= getFrameTime()) {
            mUpdate(mT, mF);
            resetTimer(true);
        }
    }

    template<typename T, typename F>
    T &DelegateAnimation<T, F>::operator*() {
        return mT;
    }

    template<typename T, typename F>
    const T &DelegateAnimation<T, F>::operator*() const {
        return mT;
    }

    template<typename T, typename F>
    T *DelegateAnimation<T, F>::operator->() {
        return &mT;
    }

    template<typename T, typename F>
    const T *DelegateAnimation<T, F>::operator->() const {
        return &mT;
    }

    template<typename T, typename F>
    void DelegateAnimation<T, F>::draw(sf::RenderTarget &target, sf::RenderStates states) const {
        (void) states;
        target.draw(mT);
    }
}
