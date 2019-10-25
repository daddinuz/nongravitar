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

#include <Canvas.hpp>

using namespace nongravitar;

void Canvas::bind(const Sprite &sprite) {
    const auto[left, top, width, height]  = sf::FloatRect(sprite.getFrame());

    mVertices[0].position = {0, 0};
    mVertices[1].position = {0, height};
    mVertices[2].position = {width, 0};
    mVertices[3].position = {width, height};

    mVertices[0].texCoords = {left, top};
    mVertices[1].texCoords = {left, top + height};
    mVertices[2].texCoords = {left + width, top};
    mVertices[3].texCoords = {left + width, top + height};

    mTexture = sprite.getTexture();
}

void Canvas::setColor(const sf::Color &color) {
    mVertices[0].color = color;
    mVertices[1].color = color;
    mVertices[2].color = color;
    mVertices[3].color = color;
}

void Canvas::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    if (mTexture) {
        states.texture = mTexture;
        target.draw(mVertices.data(), mVertices.size(), sf::TriangleStrip, states);
    }
}
