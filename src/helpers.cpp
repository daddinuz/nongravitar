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

#include "helpers.hpp"

using namespace gravitar;

float helpers::deg2rad(const float deg) {
    return deg * static_cast<float>(M_PI) / 180.0f;
}

float helpers::rad2deg(const float rad) {
    return rad * 180.0f / static_cast<float>(M_PI);
}

float helpers::rotation(const sf::Vertex &origin, const sf::Vertex &point) {
    return rotation(origin.position, point.position);
}

float helpers::rotation(const sf::Vertex &point) {
    return rotation(point.position);
}

float helpers::magnitude(const sf::Vertex &origin, const sf::Vertex &point) {
    return magnitude(origin.position, point.position);
}

float helpers::magnitude(const sf::Vertex &point) {
    return magnitude(point.position);
}

sf::Vertex helpers::normalized(const sf::Vertex &origin, const sf::Vertex &point) {
    return sf::Vertex(normalized(origin.position, point.position));
}

sf::Vertex helpers::normalized(const sf::Vertex &point) {
    return sf::Vertex(normalized(point.position));
}
