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
#include <Scene.hpp>

namespace gravitar::components {
    struct Velocity final {
        sf::Vector2f value;
    };

    struct Health final {
        int value;

        [[nodiscard]] inline bool isDead() const noexcept {
            return value <= 0;
        }
    };

    struct Fuel final {
        float value;

        [[nodiscard]] inline bool isOver() const noexcept {
            return value <= 0;
        }
    };

    class RechargeTime final {
    public:
        explicit RechargeTime(float secondsBeforeShoot);

        void reset();

        void elapse(const sf::Time &time);

        [[nodiscard]] inline bool canShoot() const noexcept {
            return mElapsed >= mSecondsBeforeShoot;
        }

    private:
        float mElapsed;
        float mSecondsBeforeShoot;
    };

    class SceneSwitcher final {
    public:
        explicit SceneSwitcher(SceneId sceneId);

        [[nodiscard]] inline SceneId getSceneId() const noexcept {
            return mSceneId;
        }

    private:
        SceneId mSceneId;
    };

    class HitRadius final {
    public:
        explicit HitRadius(float value);

        [[nodiscard]] inline float getRadius() const noexcept {
            return mRadius;
        }

    private:
        float mRadius;
    };

    class Renderable final : public sf::Drawable {
        using Instance = std::variant<sf::Sprite, sf::CircleShape>;

    public:
        explicit Renderable(sf::Sprite &&instance);
        explicit Renderable(sf::CircleShape &&instance);

        void rotate(float angle);
        [[nodiscard]] float getRotation() const noexcept;

        void move(const sf::Vector2f &offset);
        void setPosition(const sf::Vector2f &position);
        [[nodiscard]] sf::Vector2f getPosition() const noexcept;

        [[nodiscard]] sf::Vector2f getOrigin() const noexcept;

        [[nodiscard]] Instance &operator*() noexcept;
        [[nodiscard]] const Instance &operator*() const noexcept;

    private:
        void draw(sf::RenderTarget &target, sf::RenderStates states) const final;

        Instance mInstance;
    };
}
