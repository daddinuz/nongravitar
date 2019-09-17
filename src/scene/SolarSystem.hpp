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

#pragma once

#include <entt/entt.hpp>
#include <scene/Scene.hpp>
#include <assets/SpriteSheetManager.hpp>

namespace gravitar::scene {
    class SolarSystem final : public Scene {
    public:
        SolarSystem() = delete; // no default-constructible

        explicit SolarSystem(const assets::SpriteSheetManager &spriteSheetManager);

        SolarSystem(const SolarSystem &) = delete; // no copy-constructible
        SolarSystem &operator=(const SolarSystem &) = delete; // no copy-assignable

        SolarSystem(SolarSystem &&) = delete; // no move-constructible
        SolarSystem &operator=(SolarSystem &&) = delete; // no move-assignable

        void update(const sf::RenderWindow &window, sf::Time elapsed) final;

        void render(sf::RenderTarget &renderTarget) final;

    private:
        void inputSystem(const sf::RenderWindow &window, sf::Time elapsed);
        void motionSystem(sf::Time elapsed);
        void collisionSystem(const sf::RenderWindow &window);

        entt::registry mRegistry;
        const assets::SpriteSheetManager &mSpriteSheetManager;
    };
}
