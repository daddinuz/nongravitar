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

#include <components.hpp>

using namespace nongravitar::components;

/*
 * Damage
 */

Damage::Damage(const int value) {
    mValue = std::max(value, 0);
}

/*
 * Health
 */

Health::Health(const int value) {
    mValue = std::max(value, 0);
}

void Health::heal(const Supply<Health> &supply) {
    mValue += std::max(supply->getValue(), 0);
}

void Health::harm(const Damage &damage) {
    mValue = std::max(mValue - damage.getValue(), 0);
}

void Health::kill() {
    mValue = 0;
}

/*
 * Energy
 */

Energy::Energy(const float value) {
    mValue = std::max(value, 0.0f);
}

void Energy::recharge(const Supply<Energy> &supply) {
    mValue += std::max(supply->getValue(), 0.0f);
}

void Energy::consume(const float value) {
    mValue = std::max(mValue - value, 0.0f);
}

/*
 * ReloadTime
 */

ReloadTime::ReloadTime(const float secondsBeforeShoot)
        : mElapsed(secondsBeforeShoot), mSecondsBeforeShoot(secondsBeforeShoot) {}

void ReloadTime::reset() {
    mElapsed = 0;
}

void ReloadTime::elapse(sf::Time time) {
    if (not canShoot()) {
        mElapsed += time.asSeconds();
    }
}

/*
 * Transformation
 */

void Transformation::setScale(const float factorX, const float factorY) {
    mScale.x = factorX;
    mScale.y = factorY;
}

void Transformation::setScale(const sf::Vector2f &factors) {
    mScale = factors;
}

void Transformation::setOrigin(const float x, const float y) {
    mOrigin.x = x;
    mOrigin.y = y;
}

void Transformation::setOrigin(const sf::Vector2f &origin) {
    mOrigin = origin;
}

void Transformation::setPosition(const float x, const float y) {
    mPosition.x = x;
    mPosition.y = y;
}

void Transformation::setPosition(const sf::Vector2f &position) {
    mPosition = position;
}

void Transformation::setRotation(const float angle) {
    if ((mRotation = std::fmod(angle, 360.0f)) < 0.0f) {
        mRotation += 360.0f;
    }
}

void Transformation::scale(const float factorX, const float factorY) {
    mScale.x *= factorX;
    mScale.y *= factorY;
}

void Transformation::scale(const sf::Vector2f &factors) {
    mScale.x *= factors.x;
    mScale.y *= factors.y;
}

void Transformation::move(const float offsetX, const float offsetY) {
    mPosition.x += offsetX;
    mPosition.y += offsetY;
}

void Transformation::move(const sf::Vector2f &offset) {
    mPosition += offset;
}

void Transformation::rotate(const float angle) {
    setRotation(mRotation + angle);
}

sf::Transform Transformation::getTransform() const {
    // no need to cache this computation since we are going to use it once per frame

    const auto angle = -mRotation * M_PI / 180.0;
    const auto cosine = static_cast<float>(std::cos(angle));
    const auto sine = static_cast<float>(std::sin(angle));
    const auto sxc = mScale.x * cosine;
    const auto syc = mScale.y * cosine;
    const auto sxs = mScale.x * sine;
    const auto sys = mScale.y * sine;
    const auto tx = -mOrigin.x * sxc - mOrigin.y * sys + mPosition.x;
    const auto ty = mOrigin.x * sxs - mOrigin.y * syc + mPosition.y;

    return sf::Transform(sxc, sys, tx, -sxs, syc, ty, 0.0f, 0.0f, 1.0f);
}
