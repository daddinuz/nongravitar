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
#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include <Scene.hpp>

namespace nongravitar::components {
    struct Velocity final {
        sf::Vector2f value;
    };

    struct Score final {
        unsigned value;
    };

    class Damage final {
    public:
        explicit Damage(int value);

        [[nodiscard]] inline int getValue() const {
            return mValue;
        }

    private:
        int mValue;
    };

    template<typename T>
    class Supply final {
    public:
        template<typename ...Args>
        explicit Supply(Args &&... args) : mInstance{std::forward<Args>(args)...} {}

        [[nodiscard]] inline T *operator->() {
            return &mInstance;
        }

        [[nodiscard]] inline const T *operator->() const {
            return &mInstance;
        }

    private:
        T mInstance;
    };

    class Health final {
    public:
        explicit Health(int value);

        void heal(const Supply<Health> &supply);
        void harm(const Damage &damage);
        void kill();

        [[nodiscard]] inline bool isOver() const {
            return mValue <= 0;
        }

        [[nodiscard]] inline int getValue() const {
            return mValue;
        }

    private:
        int mValue;
    };

    class Energy final {
    public:
        explicit Energy(float value);

        void recharge(const Supply<Energy> &supply);
        void consume(float value);

        [[nodiscard]] inline bool isOver() const {
            return mValue <= 0.001f;
        }

        [[nodiscard]] inline float getValue() const {
            return mValue;
        }

    private:
        float mValue;
    };

    class SceneRef final {
    public:
        template<typename ...Args>
        explicit SceneRef(Args &&... args) : mInstance{std::forward<Args>(args)...} {}

        [[nodiscard]] inline SceneId operator*() const {
            return mInstance;
        }

    private:
        SceneId mInstance;
    };

    template<typename>
    class EntityRef final {
    public:
        template<typename ...Args>
        explicit EntityRef(Args &&... args) : mInstance{std::forward<Args>(args)...} {}

        [[nodiscard]] inline entt::entity operator*() const {
            return mInstance;
        }

    private:
        entt::entity mInstance;
    };

    class ReloadTime final {
    public:
        explicit ReloadTime(float secondsBeforeShoot);

        void reset();

        void elapse(sf::Time time);

        [[nodiscard]] inline bool canShoot() const {
            return mElapsed >= mSecondsBeforeShoot;
        }

    private:
        float mElapsed;
        float mSecondsBeforeShoot;
    };

    class HitRadius final {
    public:
        template<typename ...Args>
        explicit HitRadius(Args &&... args) : mInstance{std::forward<Args>(args)...} {}

        [[nodiscard]] inline float operator*() const {
            return mInstance;
        }

    private:
        float mInstance;
    };

    /// Custom implementation of transformable, basically a shrank version of the SFML's one.
    class Transformable final {
    public:
        Transformable() = default;

        void setPosition(float x, float y);

        void setPosition(const sf::Vector2f &position);

        void setRotation(float angle);

        void setOrigin(float x, float y);

        void setOrigin(const sf::Vector2f &origin);

        void move(float offsetX, float offsetY);

        void move(const sf::Vector2f &offset);

        void rotate(float angle);

        [[nodiscard]] inline const sf::Vector2f &getPosition() const {
            return mPosition;
        }

        [[nodiscard]] inline float getRotation() const {
            return mRotation;
        }

        [[nodiscard]] inline const sf::Vector2f &getOrigin() const {
            return mOrigin;
        }

        [[nodiscard]] sf::Transform getTransform() const;

    private:
        sf::Vector2f mOrigin;
        sf::Vector2f mPosition;
        float mRotation = 0.0f;
    };

    using Sprite = nongravitar::Sprite;

    class Renderable final : public sf::Drawable {
    public:
        template<typename ...Args>
        explicit Renderable(Args &&... args) : mInstance{std::forward<Args>(args)...} {}

        [[nodiscard]] sf::Transformable &operator*();
        [[nodiscard]] const sf::Transformable &operator*() const;

        [[nodiscard]] sf::Transformable *operator->();
        [[nodiscard]] const sf::Transformable *operator->() const;

        template<typename T>
        [[nodiscard]] inline T &as() {
            return std::get<T>(mInstance);
        }

        template<typename T>
        [[nodiscard]] inline const T &as() const {
            return std::get<T>(mInstance);
        }

    private:
        void draw(sf::RenderTarget &target, sf::RenderStates states) const final;

        std::variant<sf::Sprite, sf::CircleShape> mInstance;
    };
}
