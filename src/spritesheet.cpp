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

SpriteSheet::SpriteSheet(const sf::Texture &texture, const unsigned rows,
                         const unsigned columns, std::vector<Frame> &&frames) noexcept :
        mFrames(std::move(frames)), mTexture(texture), mRows(rows), mColumns(columns) {}

SpriteSheet SpriteSheet::from(const sf::Texture &texture, const unsigned rows, const unsigned columns,
                              const unsigned frameWidth, const unsigned frameHeight, sf::Vector2u startCoord) {
    const auto[textureWidth, textureHeight] = texture.getSize();
    if (startCoord.x + columns * frameWidth > textureWidth || startCoord.y + rows * frameHeight > textureHeight) {
        throw std::invalid_argument(trace("bad dimensions supplied"));
    }

    auto frames = std::vector<Frame>();
    frames.reserve(rows * columns);

    for (auto row = 0u; row < rows; ++row) {
        const auto top = startCoord.y + row * frameHeight;

        for (auto column = 0u; column < columns; ++column) {
            frames.emplace_back(startCoord.x + column * frameWidth, top, frameWidth, frameHeight);
        }
    }

    return SpriteSheet(texture, rows, columns, std::move(frames));
}

const std::vector<SpriteSheet::Frame> *SpriteSheet::operator->() const noexcept {
    return &mFrames;
}

const std::vector<SpriteSheet::Frame> &SpriteSheet::operator*() const noexcept {
    return mFrames;
}

const sf::Texture &SpriteSheet::getTexture() const noexcept {
    return mTexture;
}

const SpriteSheet::Frame &SpriteSheet::getFrame(const sf::Vector2u &frameCoord) const {
    return mFrames.at(frameCoord.x + frameCoord.y * mColumns);
}

sf::Sprite SpriteSheet::getSprite(const sf::Vector2u &frameCoord) const {
    return sf::Sprite(mTexture, getFrame(frameCoord));
}

SpriteSheet::const_iterator SpriteSheet::getFrameIterator(const sf::Vector2u &frameCoord) const {
    const auto offset = frameCoord.x + frameCoord.y * mColumns;

    if (offset >= mFrames.size()) {
        throw std::invalid_argument(trace("bad coord supplied"));
    }

    return mFrames.cbegin() + offset;
}

SpriteSheet::const_reverse_iterator SpriteSheet::getReverseFrameIterator(const sf::Vector2u &frameCoord) const {
    const auto offset = frameCoord.x + frameCoord.y * mColumns;

    if (offset >= mFrames.size()) {
        throw std::invalid_argument(trace("bad coord supplied"));
    }

    return mFrames.crbegin() + offset;
}
