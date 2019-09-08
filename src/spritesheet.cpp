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

SpriteSheet::SpriteSheet(const sf::Texture &texture, std::vector<Frame> &&frames, sf::Vector2u size) noexcept :
        mFrames(std::move(frames)), mTexture(texture), mSize(size) {}

SpriteSheet SpriteSheet::from(const sf::Texture &texture, const unsigned frameWidth, const unsigned frameHeight,
                              const sf::Vector2u size, const sf::Vector2u startCoord) {
    const auto[textureWidth, textureHeight] = texture.getSize();
    const auto[columns, rows] = size;

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

    return SpriteSheet(texture, std::move(frames), size);
}

SpriteSheet SpriteSheet::from(const sf::Texture &texture, const unsigned frameWidth, const unsigned frameHeight,
                              const sf::Vector2u startCoord) {
    const auto[textureWidth, textureHeight] = texture.getSize();
    return SpriteSheet::from(texture, frameWidth, frameHeight, {textureWidth / frameWidth, textureHeight / frameHeight}, startCoord);
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
    return mFrames.at(getFrameIndex(frameCoord));
}

sf::Sprite SpriteSheet::getSprite(const sf::Vector2u &frameCoord) const {
    return sf::Sprite(mTexture, getFrame(frameCoord));
}

const sf::Vector2u &SpriteSheet::getSize() const noexcept {
    return mSize;
}

SpriteSheet::const_iterator SpriteSheet::getFrameIterator(const sf::Vector2u &frameCoord) const {
    const auto offset = getFrameIndex(frameCoord);

    if (offset >= mFrames.size()) {
        throw std::invalid_argument(trace("bad coord supplied"));
    }

    return mFrames.cbegin() + offset;
}

SpriteSheet::const_reverse_iterator SpriteSheet::getReverseFrameIterator(const sf::Vector2u &frameCoord) const {
    const auto offset = getFrameIndex(frameCoord);

    if (offset >= mFrames.size()) {
        throw std::invalid_argument(trace("bad coord supplied"));
    }

    return mFrames.crbegin() + offset;
}

std::size_t SpriteSheet::getFrameIndex(const sf::Vector2u &frameCoord) const noexcept {
    return frameCoord.x + frameCoord.y * mSize.x;
}
