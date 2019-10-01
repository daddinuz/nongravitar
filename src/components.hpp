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
    class Velocity final {
    public:
        template<typename ...Args>
        explicit Velocity(Args &&... args) : mInstance(std::forward<Args>(args)...) {}

        [[nodiscard]] inline sf::Vector2f &operator*() noexcept {
            return mInstance;
        }

        [[nodiscard]] inline const sf::Vector2f &operator*() const noexcept {
            return mInstance;
        }

    private:
        sf::Vector2f mInstance;
    };

    class Health final {
    public:
        template<typename ...Args>
        explicit Health(Args &&... args) : mInstance(std::forward<Args>(args)...) {}

        [[nodiscard]] inline int &operator*() noexcept {
            return mInstance;
        }

        [[nodiscard]] inline const int &operator*() const noexcept {
            return mInstance;
        }

        [[nodiscard]] inline bool isDead() const noexcept {
            return mInstance <= 0;
        }

    private:
        int mInstance;
    };

    class Fuel final {
    public:
        template<typename ...Args>
        explicit Fuel(Args &&... args) : mInstance(std::forward<Args>(args)...) {}

        [[nodiscard]] inline float &operator*() noexcept {
            return mInstance;
        }

        [[nodiscard]] inline const float &operator*() const noexcept {
            return mInstance;
        }

        [[nodiscard]] inline bool isOver() const noexcept {
            return mInstance <= 0.0f;
        }

    private:
        float mInstance;
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

        [[nodiscard]] inline SceneId sceneId() const noexcept {
            return mSceneId;
        }

    private:
        SceneId mSceneId;
    };

    class HitRadius final {
    public:
        template<typename ...Args>
        explicit HitRadius(Args &&... args) : mInstance(std::forward<Args>(args)...) {}

        [[nodiscard]] inline const float &operator*() const noexcept {
            return mInstance;
        }

    private:
        float mInstance;
    };

    class Renderable final : public sf::Drawable {
    public:
        explicit Renderable(sf::Sprite &&instance);
        explicit Renderable(sf::CircleShape &&instance);

        [[nodiscard]] sf::Transformable &operator*() noexcept;
        [[nodiscard]] const sf::Transformable &operator*() const noexcept;

        [[nodiscard]] sf::Transformable *operator->() noexcept;
        [[nodiscard]] const sf::Transformable *operator->() const noexcept;

        template<typename T>
        [[nodiscard]] inline T &as() noexcept {
            return std::get<T>(mInstance);
        }

        template<typename T>
        [[nodiscard]] inline const T &as() const noexcept {
            return std::get<T>(mInstance);
        }

    private:
        void draw(sf::RenderTarget &target, sf::RenderStates states) const final;

        std::variant<sf::Sprite, sf::CircleShape> mInstance;
    };
}
