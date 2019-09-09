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

// TODO move definitions at the bottom of the file
namespace gravitar::helpers {
    template<typename T>
    std::ostream &operator<<(std::ostream &os, const sf::Vector2<T> &obj) {
        return os << "Vector2<" << typeid(T).name() << ">(" << obj.x << ", " << obj.y << ')';
    }

    template<typename T>
    std::ostream &operator<<(std::ostream &os, const sf::Rect<T> &obj) {
        return os << "Rect<" << typeid(T).name() << ">(" << obj.top << ", " << obj.left << ", " << obj.width << ", " << obj.height << ')';
    }

    std::ostream &operator<<(std::ostream &os, const sf::Vertex &obj);

    float deg2rad(float deg);

    float rad2deg(float rad);

    template<typename T>
    inline constexpr int signum(T n, std::false_type) {
        return T(0) < n;
    }

    template<typename T>
    inline constexpr int signum(T n, std::true_type) {
        return (T(0) < n) - (n < T(0));
    }

    template<typename T>
    inline constexpr int signum(T n) {
        return signum(n, std::is_signed<T>());
    }

    /// Range [0, 360].
    template<typename T>
    float rotation(const sf::Vector2<T> &origin, const sf::Vector2<T> &point) {
        return std::fmod(rad2deg(std::atan2(point.y - origin.y, point.x - origin.x)) + 360.0f, 361.0f);
    }

    template<typename T>
    float rotation(const sf::Vector2<T> &point) {
        return rotation<T>({0, 0}, point);
    }

    float rotation(const sf::Vertex &origin, const sf::Vertex &point);

    float rotation(const sf::Vertex &point);

    /// Range [-180, 180].
    float shortestRotation(float currentBearing, float targetBearing);

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
    sf::Vector2<T> makeVector2(float angle, const T magnitude) {
        angle = deg2rad(angle);
        return sf::Vector2<T>(std::cos(angle), std::sin(angle)) * magnitude;
    }

    template<typename T>
    class Wrapper {
    public:
        template<typename ...Args>
        explicit Wrapper(Args &&... args) : mInner(std::forward<Args>(args)...) {}

        Wrapper(Wrapper<T> &&) noexcept = default; // move-constructible required because of entt registry
        Wrapper &operator=(Wrapper<T> &&) noexcept = default; // move-assignable required because of entt registry

        [[nodiscard]] T &operator*() noexcept {
            return mInner;
        }

        [[nodiscard]] const T &operator*() const noexcept {
            return mInner;
        }

        [[nodiscard]] T *operator->() noexcept {
            return &mInner;
        }

        [[nodiscard]] const T *operator->() const noexcept {
            return &mInner;
        }

        virtual ~Wrapper() = 0;

    private:
        T mInner;
    };

    /*
     * Implementation
     */

    template<typename T>
    Wrapper<T>::~Wrapper() = default;
}
