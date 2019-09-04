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

using namespace gravitar;

void Animation::setLoop(bool loop) {
    mLoop = loop;
}

bool Animation::getLoop() const {
    return mLoop;
}

void Animation::resetTimer(bool keepReminder) {
    if (keepReminder) {
        mTimer = sf::microseconds(mTimer.asMicroseconds() % mFrameTime.asMicroseconds());
    } else {
        mTimer = sf::Time::Zero;
    }
}

const sf::Time &Animation::elapse(const sf::Time &time) {
    return mTimer += time;
}

const sf::Time &Animation::getTimer() const {
    return mTimer;
}

void Animation::setFrameTime(sf::Time time) {
    mFrameTime = time;
}

const sf::Time &Animation::getFrameTime() const {
    return mFrameTime;
}

SpriteAnimation::SpriteAnimation(const sf::Texture &texture) : SpriteSheet(texture) {}

SpriteAnimation SpriteAnimation::from(const sf::Texture &texture, sf::Vector2i startCoord, sf::Vector2i table, sf::Vector2i frame, sf::Time frameTime) {
    return SpriteAnimation(texture, startCoord, table, frame, frameTime);
}

void SpriteAnimation::update(const sf::Time &time) {
    if (elapse(time) >= getFrameTime()) {
        const auto nextFrameId = getCurrentFrameId() + 1;
        const auto frames = getFrames().size();

        if (nextFrameId < frames) {
            setFrame(nextFrameId);
        } else if (getLoop() and frames > 0) {
            setFrame(0);
        }

        resetTimer(true);
    }
}

SpriteAnimation::SpriteAnimation(const sf::Texture &texture, sf::Vector2i startCoord, sf::Vector2i table, sf::Vector2i frame, sf::Time frameTime)
        : SpriteSheet(texture, startCoord, table, frame) {
    setFrameTime(frameTime);
}
