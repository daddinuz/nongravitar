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

#include "spritesheet.hpp"
#include "trace.hpp"

using namespace gravitar;

SpriteSheet::SpriteSheet(const sf::Texture &texture, Buffer &&buffer, sf::Vector2u size) noexcept
        : mBuffer(std::move(buffer)), mTexture(texture), mSize(size) {}

SpriteSheet SpriteSheet::from(const sf::Texture &texture, const sf::Vector2u frame, const sf::Vector2u startCoord) {
    const auto[textureWidth, textureHeight] = texture.getSize();
    const auto columns = textureWidth / frame.x, rows = textureHeight / frame.y;

    if (startCoord.x + columns * frame.x > textureWidth || startCoord.y + rows * frame.y > textureHeight) {
        throw std::invalid_argument(trace("bad dimensions supplied"));
    }

    auto buffer = Buffer();
    buffer.reserve(rows * columns);

    for (auto row = 0u; row < rows; ++row) {
        const auto top = startCoord.y + row * frame.y;

        for (auto column = 0u; column < columns; ++column) {
            buffer.emplace_back(startCoord.x + column * frame.x, top, frame.x, frame.y);
        }
    }

    return SpriteSheet(texture, std::move(buffer), {columns, rows});
}

SpriteSheet::const_iterator SpriteSheet::cbegin() const noexcept {
    return mBuffer.cbegin();
}

SpriteSheet::const_iterator SpriteSheet::cend() const noexcept {
    return mBuffer.cend();
}

const SpriteSheet::Buffer &SpriteSheet::getBuffer() const noexcept {
    return mBuffer;
}

const sf::Texture &SpriteSheet::getTexture() const noexcept {
    return mTexture;
}

const SpriteSheet::Frame &SpriteSheet::getFrame(const sf::Vector2u &frameCoord) const {
    return mBuffer.at(frameIndex(frameCoord));
}

sf::Sprite SpriteSheet::instanceSprite(const sf::Vector2u &frameCoord) const {
    return sf::Sprite(mTexture, getFrame(frameCoord));
}

const sf::Vector2u &SpriteSheet::getSize() const noexcept {
    return mSize;
}

std::size_t SpriteSheet::frameIndex(const sf::Vector2u &frameCoord) const noexcept {
    return frameCoord.x + frameCoord.y * mSize.x;
}
