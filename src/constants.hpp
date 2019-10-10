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

namespace gravitar::constants {
    inline constexpr auto FPS = 65u;

    inline constexpr auto PLAYER_HEALTH = 6;
    inline constexpr auto PLAYER_ENERGY = 20000.0f;
    inline constexpr auto PLAYER_RELOAD_TIME = 0.38f;
    inline constexpr auto PLAYER_SPEED = 220.0f;
    inline constexpr auto PLAYER_ROTATION_SPEED = 220.0f;

    inline constexpr auto TRACTOR_RADIUS = 64.0f;

    inline constexpr auto BULLET_SPEED = 1024.0f;

    inline constexpr auto PLANETS = 8u;
    inline constexpr std::array<sf::Uint32, 16> PLANET_COLORS = {
            0xF0F8FF,
            0xE6E6FA,
            0x87CEFA,
            0x5F9EA0,
            0x6A5ACD,
            0x7FFFD4,
            0xAFEEEE,
            0x40E0D0,
            0x00CED1,
            0x800000,
            0xB22222,
            0xFFB6C1,
            0xFF1493,
            0xFFDAB9,
            0xF0E68C,
            0xADFF2F,
    };
}
