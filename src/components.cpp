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
