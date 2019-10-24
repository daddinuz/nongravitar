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
#include <Scene.hpp>
#include <pubsub.hpp>
#include <helpers.hpp>
#include <messages.hpp>

namespace nongravitar::scene {
    class PlanetAssault final : public Scene, public pubsub::Handler<messages::PlanetEntered> {
    public:
        PlanetAssault() = delete; // no default-constructible

        PlanetAssault(SceneId solarSystemSceneId, SceneId leaderBoardSceneId, sf::Color terrainColor);

        PlanetAssault(const PlanetAssault &) = delete; // no copy-constructible
        PlanetAssault &operator=(const PlanetAssault &) = delete; // no copy-assignable

        PlanetAssault(PlanetAssault &&) = delete; // no move-constructible
        PlanetAssault &operator=(PlanetAssault &&) = delete; // no move-assignable

        SceneId update(const sf::RenderWindow &window, SceneManager &sceneManager, Assets &assets, sf::Time elapsed) final;

        void render(sf::RenderTarget &window) const final;

    private:
        Scene &setup(const sf::RenderWindow &window, Assets &assets) final;

        void operator()(const messages::PlanetEntered &message) final;

        void initializeGroups();
        void initializeTerrain(const sf::RenderWindow &window, Assets &assets, sf::Color terrainColor);

        void inputSystem(Assets &assets, sf::Time elapsed);
        void motionSystem(sf::Time elapsed);
        void collisionSystem(const sf::RenderWindow &window, Assets &assets, sf::Time elapsed);
        void reloadSystem(sf::Time elapsed);
        void AISystem(Assets &assets);
        void livenessSystem(Assets &assets);
        void reportSystem(const sf::RenderWindow &window);

        entt::registry mRegistry;
        char mBuffer[56];
        sf::Text mReport;
        helpers::RandomEngine mRandomEngine;
        const SceneId mLeaderBoardSceneId;
        const SceneId mSolarSystemSceneId;
        SceneId mNextSceneId = nullSceneId;
        const sf::Color mTerrainColor;
        unsigned mBonus{0u};
    };
}
