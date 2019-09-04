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

using namespace gravitar;

SpriteSheet::SpriteSheet(const sf::Texture &texture) : mSprite(texture) {}

SpriteSheet SpriteSheet::from(const sf::Texture &texture, sf::Vector2i startCoord, sf::Vector2i table, sf::Vector2i frame) {
    return SpriteSheet(texture, startCoord, table, frame);
}

SpriteSheet::FrameId SpriteSheet::addFrame(Frame frame) {
    mFrames.push_back(frame);
    return mFrames.size() - 1;
}

void SpriteSheet::setFrame(FrameId frameId) {
    const auto frame = mFrames.at(frameId);

    mCurrentFrameId = frameId;
    mSprite.setTextureRect(frame);
}

SpriteSheet::FrameId SpriteSheet::getCurrentFrameId() const {
    return mCurrentFrameId;
}

const SpriteSheet::Frame &SpriteSheet::getCurrentFrame() const {
    return getFrame(getCurrentFrameId());
}

const SpriteSheet::Frame &SpriteSheet::getFrame(FrameId frameId) const {
    return mFrames.at(frameId);
}

const std::vector<SpriteSheet::Frame> &SpriteSheet::getFrames() const {
    return mFrames;
}

void SpriteSheet::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    if (mCurrentFrameId < mFrames.size()) {
        states.transform *= getTransform();
        target.draw(mSprite, states);
    }
}

sf::FloatRect SpriteSheet::getLocalBounds() const {
    return mSprite.getLocalBounds();
}

sf::FloatRect SpriteSheet::getGlobalBounds() const {
    return getTransform().transformRect(getLocalBounds());
}

SpriteSheet::SpriteSheet(const sf::Texture &texture, sf::Vector2i startCoord, sf::Vector2i table, sf::Vector2i frame) : mSprite(texture) {
    for (auto y = 0; y < table.y; y++) {
        const auto top = startCoord.y + y * frame.y;

        for (auto x = 0; x < table.x; x++) {
            addFrame(startCoord.x + x * frame.x, top, frame.x, frame.y);
        }
    }

    if (not mFrames.empty()) {
        setFrame(0);
    }
}
