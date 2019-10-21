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

using namespace nongravitar::assets;
using namespace nongravitar::components;

/*
 * Damage
 */

Damage::Damage(const int value) noexcept {
    mValue = std::max(value, 0);
}

int Damage::getValue() const noexcept {
    return mValue;
}

/*
 * Health
 */

Health::Health(const int value) noexcept {
    mValue = std::max(value, 0);
}

void Health::heal(const Supply<Health> &supply) noexcept {
    mValue += std::max(supply->getValue(), 0);
}

void Health::harm(const Damage &damage) noexcept {
    mValue = std::max(mValue - damage.getValue(), 0);
}

void Health::kill() noexcept {
    mValue = 0;
}

bool Health::isOver() const noexcept {
    return mValue <= 0;
}

int Health::getValue() const noexcept {
    return mValue;
}

/*
 * Energy
 */

Energy::Energy(const float value) noexcept {
    mValue = std::max(value, 0.0f);
}

void Energy::recharge(const Supply<Energy> &supply) noexcept {
    mValue += std::max(supply->getValue(), 0.0f);
}

void Energy::consume(const float value) noexcept {
    mValue = std::max(mValue - value, 0.0f);
}

bool Energy::isOver() const noexcept {
    return mValue <= 0.001f;
}

float Energy::getValue() const noexcept {
    return mValue;
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
 * SpaceShipEngineAnimation
 */

SpaceShipEngineAnimation::SpaceShipEngineAnimation(Assets &assets) {
    const auto &sheet = assets.getSpriteSheetsManager().get(SpriteSheetId::SpaceShip);

    mDelegate.addStep(sheet.getRect({0, 0}), sf::seconds(0.1f));
    mDelegate.addStep(sheet.getRect({1, 0}), sf::seconds(0.1f));
    mDelegate.addStep(sheet.getRect({2, 0}), sf::seconds(0.1f));
    mDelegate.addStep(sheet.getRect({3, 0}), sf::seconds(0.1f));
    mDelegate.addStep(sheet.getRect({4, 0}), sf::seconds(0.1f));
    mDelegate.addStep(sheet.getRect({5, 0}), sf::seconds(0.1f));
    mDelegate.addStep(sheet.getRect({6, 0}), sf::seconds(0.1f));
    mDelegate.addStep(sheet.getRect({7, 0}), sf::seconds(0.1f));
}

/*
 * Renderable
 */

void Renderable::draw(sf::RenderTarget &target, sf::RenderStates) const {
    std::visit([&target](const auto &instance) { target.draw(instance); }, mInstance);
}

sf::Transformable &Renderable::operator*() {
    return std::visit([](auto &instance) -> sf::Transformable & { return instance; }, mInstance);
}

const sf::Transformable &Renderable::operator*() const {
    return std::visit([](const auto &instance) -> const sf::Transformable & { return instance; }, mInstance);
}

sf::Transformable *Renderable::operator->() {
    return std::visit([](auto &instance) -> sf::Transformable * { return &instance; }, mInstance);
}

const sf::Transformable *Renderable::operator->() const {
    return std::visit([](const auto &instance) -> const sf::Transformable * { return &instance; }, mInstance);
}
