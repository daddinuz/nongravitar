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

#include <cmath>

#include <SFML/Graphics.hpp>

namespace gravitar::helpers {
    float deg2rad(float deg);

    float rad2deg(float rad);

    template<typename T>
    float rotation(const sf::Vector2<T> &origin, const sf::Vector2<T> &point) {
        return std::fmod(rad2deg(std::atan2(point.y - origin.y, point.x - origin.x)) + 359.0f, 359.0f);
    }

    template<typename T>
    float rotation(const sf::Vector2<T> &point) {
        return rotation<T>({0, 0}, point);
    }

    float rotation(const sf::Vertex &origin, const sf::Vertex &point);

    float rotation(const sf::Vertex &point);

    template<typename T>
    float magnitude(const sf::Vector2<T> &origin, const sf::Vector2<T> &point) {
        return std::sqrt(std::pow(point.x - origin.x, 2) + std::pow(point.y - origin.y, 2));
    }

    template<typename T>
    float magnitude(const sf::Vector2<T> &point) {
        return magnitude<T>({0, 0}, point);
    }

    float magnitude(const sf::Vertex &origin, const sf::Vertex &point);

    float magnitude(const sf::Vertex &point);

    template<typename T>
    sf::Vector2<T> normalized(const sf::Vector2<T> &origin, const sf::Vector2<T> &point) {
        decltype(auto) diff = point - origin;

        if (decltype(auto) mag = magnitude<T>(origin, point); mag > 0) {
            diff /= mag;
        }

        return diff;
    }

    template<typename T>
    sf::Vector2<T> normalized(const sf::Vector2<T> &point) {
        return normalized<T>({0, 0}, point);
    }

    sf::Vertex normalized(const sf::Vertex &origin, const sf::Vertex &point);

    sf::Vertex normalized(const sf::Vertex &point);

    template<typename T>
    class Vec2 final {
    public:
        using Type = sf::Vector2<T>;

        Vec2() = delete;

        template<typename ...Args>
        explicit Vec2(Args &&... args);

        Vec2(const Vec2 &) = delete;
        Vec2 &operator=(const Vec2 &) = delete;

        Vec2(Vec2 &&) = delete;
        Vec2 &operator=(Vec2 &&) = delete;

        [[nodiscard]] Type &operator*() noexcept;
        [[nodiscard]] const Type &operator*() const noexcept;

        [[nodiscard]] Type *operator->() noexcept;
        [[nodiscard]] const Type *operator->() const noexcept;

        [[nodiscard]] float getRotation() const noexcept;
        [[nodiscard]] float getRotation(const Type &other) const noexcept;

        [[nodiscard]] float getMagnitude() const noexcept;
        [[nodiscard]] float getMagnitude(const Type &other) const noexcept;

        void rotate(float angle) noexcept;
        void setRotation(float angle) noexcept;

        [[nodiscard]] Type &normalize() noexcept;
        [[nodiscard]] Type &normalize(const Type &other) noexcept;

    private:
        Type mInner;
    };

    /*
     * Implementation
     */

    template<typename T>
    template<typename... Args>
    Vec2<T>::Vec2(Args &&... args) :
            mInner(std::forward<Args>(args)...) {}

    template<typename T>
    typename Vec2<T>::Type &Vec2<T>::operator*() noexcept {
        return mInner;
    }

    template<typename T>
    const typename Vec2<T>::Type &Vec2<T>::operator*() const noexcept {
        return mInner;
    }

    template<typename T>
    typename Vec2<T>::Type *Vec2<T>::operator->() noexcept {
        return &mInner;
    }

    template<typename T>
    const typename Vec2<T>::Type *Vec2<T>::operator->() const noexcept {
        return &mInner;
    }

    template<typename T>
    float Vec2<T>::getRotation() const noexcept {
        return helpers::rotation(mInner);
    }

    template<typename T>
    float Vec2<T>::getRotation(const Type &other) const noexcept {
        return helpers::rotation(mInner, other);
    }

    template<typename T>
    float Vec2<T>::getMagnitude() const noexcept {
        return helpers::magnitude(mInner);
    }

    template<typename T>
    float Vec2<T>::getMagnitude(const Type &other) const noexcept {
        return helpers::magnitude(mInner, other);
    }

    template<typename T>
    void Vec2<T>::rotate(float angle) noexcept {
        angle = helpers::deg2rad(getRotation()) + helpers::deg2rad(angle);
        mInner = sf::Vector2<T>(std::cos(angle), std::sin(angle)) * getMagnitude();
    }

    template<typename T>
    void Vec2<T>::setRotation(float angle) noexcept {
        angle = helpers::deg2rad(angle);
        mInner = sf::Vector2<T>(std::cos(angle), std::sin(angle)) * getMagnitude();
    }

    template<typename T>
    typename Vec2<T>::Type &Vec2<T>::normalize() noexcept {
        mInner = helpers::normalized(mInner);
        return mInner;
    }

    template<typename T>
    typename Vec2<T>::Type &Vec2<T>::normalize(const Vec2::Type &other) noexcept {
        mInner = helpers::normalized(mInner, other);
        return mInner;
    }
}
