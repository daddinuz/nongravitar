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

#include "animation.hpp"

gravitar::SpriteAnimation::SpriteAnimation(const sf::Texture &texture) : mTexture{texture} {
    setFrame(0, false);
}

gravitar::SpriteAnimation::SpriteAnimation(const sf::Texture &texture, sf::Vector2i coord, sf::Vector2i table, sf::Vector2i frame, sf::Time frameTime)
        : mTexture{texture}, mFrameTime{frameTime} {
    for (auto y = 0; y < table.y; y++) {
        const auto top = static_cast<int>(coord.y + y * frame.y);

        for (auto x = 0; x < table.x; x++) {
            addFrame({static_cast<int>(coord.x + x * frame.x), top, frame.x, frame.y});
        }
    }

    setFrame(0, false);
}

void gravitar::SpriteAnimation::update(const sf::Time &elapsed) {
    mElapsed += elapsed;

    if (mElapsed >= mFrameTime) {
        // reset time, but keep the remainder
        mElapsed = sf::microseconds(mElapsed.asMicroseconds() % mFrameTime.asMicroseconds());

        if (++mCurrentFrame >= mFrames.size()) {
            mCurrentFrame -= mLoop ? mCurrentFrame : 1;
        }

        setFrame(mCurrentFrame, false);
    }
}

sf::FloatRect gravitar::SpriteAnimation::getLocalBounds() const {
    const auto &rect = mFrames.at(mCurrentFrame);
    const auto width = static_cast<float>(std::abs(rect.width));
    const auto height = static_cast<float>(std::abs(rect.height));

    return sf::FloatRect(0.f, 0.f, width, height);
}

sf::FloatRect gravitar::SpriteAnimation::getGlobalBounds() const {
    return getTransform().transformRect(getLocalBounds());
}

size_t gravitar::SpriteAnimation::addFrame(sf::IntRect frame) {
    mFrames.push_back(frame);
    return mFrames.size() - 1;
}

void gravitar::SpriteAnimation::setFrame(size_t frame, bool resetTime) {
    const auto rect = mFrames.at(frame);
    const auto left = static_cast<float>(rect.left);
    const auto top = static_cast<float>(rect.top);
    const auto width = static_cast<float>(rect.width);
    const auto height = static_cast<float>(rect.height);

    mVertices[0].position = sf::Vector2f(0.0f, 0.0f);
    mVertices[1].position = sf::Vector2f(width, 0.0f);
    mVertices[2].position = sf::Vector2f(width, height);
    mVertices[3].position = sf::Vector2f(0.f, height);

    mVertices[0].texCoords = sf::Vector2f(left, top);
    mVertices[1].texCoords = sf::Vector2f(left + width, top);
    mVertices[2].texCoords = sf::Vector2f(left + width, top + height);
    mVertices[3].texCoords = sf::Vector2f(left, top + height);

    mCurrentFrame = frame;

    if (resetTime) {
        mElapsed = sf::Time::Zero;
    }
}

const sf::IntRect &gravitar::SpriteAnimation::getFrame(size_t frame) const {
    return mFrames.at(frame);
}

const std::vector<sf::IntRect> &gravitar::SpriteAnimation::frames() const {
    return mFrames;
}

void gravitar::SpriteAnimation::setFrameTime(sf::Time frameTime) {
    mFrameTime = frameTime;
}

const sf::Time &gravitar::SpriteAnimation::getFrameTime() const {
    return mFrameTime;
}

void gravitar::SpriteAnimation::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &mTexture;
    target.draw(mVertices, 4, sf::Quads, states);
}

void gravitar::SpriteAnimation::setLoop(bool loop) {
    mLoop = loop;
}
