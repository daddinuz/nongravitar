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

#include <variant>
#include <SFML/Graphics.hpp>
#include <Deref.hpp>
#include <Scene.hpp>

namespace gravitar::components {
    struct Velocity final : public Deref<sf::Vector2f> {
        template<typename ...Args>
        explicit Velocity(Args &&... args) : Deref(std::forward<Args>(args)...) {}
    };

    struct Health final : public Deref<int> {
        template<typename ...Args>
        explicit Health(Args &&... args) : Deref(std::forward<Args>(args)...) {}

        [[nodiscard]] inline bool isDead() const noexcept {
            return this->deref() <= 0;
        }
    };

    struct Fuel final : public Deref<float> {
        template<typename ...Args>
        explicit Fuel(Args &&... args) : Deref(std::forward<Args>(args)...) {}

        [[nodiscard]] inline bool isOver() const noexcept {
            return this->deref() <= 0;
        }
    };

    class RechargeTime final {
    public:
        explicit RechargeTime(float secondsBeforeShoot);

        void reset();

        void elapse(const sf::Time &time);

        [[nodiscard]] bool canShoot() const noexcept;

    private:
        float mElapsed;
        float mSecondsBeforeShoot;
    };

    struct SceneSwitcher final : public Deref<SceneId> {
        template<typename ...Args>
        explicit SceneSwitcher(Args &&... args) : Deref(std::forward<Args>(args)...) {}
    };

    class Renderable final : public sf::Drawable {
    public:
        explicit Renderable(sf::Sprite &&instance);
        explicit Renderable(sf::CircleShape &&instance);

        void rotate(float angle);
        [[nodiscard]] float getRotation() const noexcept;

        void move(const sf::Vector2f &offset);
        void setPosition(const sf::Vector2f &position);
        [[nodiscard]] sf::Vector2f getPosition() const noexcept;

        [[nodiscard]] sf::Vector2f getOrigin() const noexcept;
        [[nodiscard]] sf::FloatRect getHitBox() const noexcept;

    private:
        void draw(sf::RenderTarget &target, sf::RenderStates states) const final;

        std::variant<sf::Sprite, sf::CircleShape> mInstance;
    };
}
