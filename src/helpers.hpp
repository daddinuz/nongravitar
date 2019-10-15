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
#include <random>
#include <SFML/Graphics.hpp>

namespace nongravitar::helpers {
    using RandomDevice = std::random_device;
    using RandomEngine = std::default_random_engine;
    using IntDistribution = std::uniform_int_distribution<int>;
    using FloatDistribution = std::uniform_real_distribution<float>;

    template<typename F, typename ...Args>
    inline constexpr void debug([[maybe_unused]] F &&f, [[maybe_unused]] Args &&...args) {
#ifndef NDEBUG
        f(std::forward<Args>(args)...);
#endif
    }

    float deg2rad(float deg);

    float rad2deg(float rad);

    template<typename T>
    void centerOrigin(sf::Transformable &self, const sf::Rect<T> &bounds) {
        self.setOrigin(bounds.left + bounds.width / T(2), bounds.top + bounds.height / T(2));
    }

    template<typename T>
    auto enumValue(T instance) {
        static_assert(std::is_enum<T>::value);
        return static_cast<typename std::underlying_type<T>::type>(instance);
    }

    /// Range [0, 360].
    template<typename T>
    float rotation(const sf::Vector2<T> &origin, const sf::Vector2<T> &point) {
        return std::fmod(rad2deg(std::atan2(point.y - origin.y, point.x - origin.x)) + 360.0f, 361.0f);
    }

    template<typename T>
    float magnitude(const sf::Vector2<T> &origin, const sf::Vector2<T> &point) {
        return std::sqrt(std::pow(point.x - origin.x, 2) + std::pow(point.y - origin.y, 2));
    }

    template<typename T>
    sf::Vector2<T> makeVector2(float angle, const T magnitude) {
        angle = deg2rad(angle);
        return sf::Vector2<T>(std::cos(angle), std::sin(angle)) * magnitude;
    }
}
