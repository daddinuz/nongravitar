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

#include <SFML/Graphics.hpp>

namespace nongravitar::constants {
    inline constexpr auto FPS = 65u;

    inline constexpr auto PLAYER_HEALTH = 5;
    inline constexpr auto PLAYER_ENERGY = 18'000.0f;
    inline constexpr auto PLAYER_RELOAD_TIME = 0.38f;
    inline constexpr auto PLAYER_SPEED = 220.0f;
    inline constexpr auto PLAYER_ROTATION_SPEED = 220.0f;

    inline constexpr auto TRACTOR_RADIUS = 64.0f;

    inline constexpr auto BULLET_SPEED = 1'024.0f;

    inline constexpr auto SCORE_PER_HIT = 10u;
    inline constexpr auto SCORE_PER_AI1 = 20u;
    inline constexpr auto SCORE_PER_AI2 = 25u;
    inline constexpr auto SCORE_PER_SOLAR_SYSTEM = 2'000u;

    inline constexpr auto PLANETS = 8u;
    inline constexpr std::array<std::array<sf::Uint8, 3>, 16> PLANET_COLORS{
            {
                    {248, 131, 121}, {218, 44, 67}, {227, 66, 52}, {86, 130, 3},
                    {255, 145, 164}, {0, 166, 147}, {62, 180, 137}, {0, 204, 153},
                    {0, 250, 154}, {170, 240, 209}, {13, 152, 186}, {150, 222, 209},
                    {135, 206, 235}, {158, 185, 212}, {0, 191, 255}, {128, 255, 212}
            }
    };
}
