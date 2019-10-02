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

#include <random>
#include <entt/entt.hpp>
#include <Scene.hpp>
#include <pubsub.hpp>
#include <messages.hpp>

namespace gravitar::scene {
    class PlanetAssault final : public pubsub::Handler<messages::PlanetEntered>, public Scene {
    public:
        PlanetAssault() = delete; // no default-constructible

        PlanetAssault(SceneId gameOverSceneId, Assets &assets);

        PlanetAssault(const PlanetAssault &) = delete; // no copy-constructible
        PlanetAssault &operator=(const PlanetAssault &) = delete; // no copy-assignable

        PlanetAssault(PlanetAssault &&) = delete; // no move-constructible
        PlanetAssault &operator=(PlanetAssault &&) = delete; // no move-assignable

        void initialize(const sf::RenderWindow &window, Assets &assets, std::mt19937 &randomEngine) noexcept;

        SceneId update(const sf::RenderWindow &window, Assets &assets, sf::Time elapsed) noexcept final;

        void render(sf::RenderTarget &window) noexcept final;

        void setParentSceneId(SceneId parentSceneId) noexcept;

        [[nodiscard]] SceneId getParentSceneId() const noexcept;

    private:
        void operator()(const messages::PlanetEntered &planetEntered) noexcept final;

        void inputSystem(const sf::RenderWindow &window, Assets &assets, sf::Time elapsed) noexcept;
        void motionSystem(sf::Time elapsed) noexcept;
        void collisionSystem(const sf::RenderWindow &window) noexcept;
        void livenessSystem() noexcept;
        void reportSystem(const sf::RenderWindow &window) noexcept;

        entt::registry mRegistry;
        sf::Text mReport;
        char mBuffer[128];

        const SceneId mGameOverSceneId;
        SceneId mParentSceneId = nullSceneId;
        SceneId mNextSceneId = nullSceneId;
    };
}
