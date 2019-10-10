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

using namespace gravitar::components;

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

void Renderable::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    (void) states;
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
