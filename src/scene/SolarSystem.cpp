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

#include <iostream>
#include <tags.hpp>
#include <trace.hpp>
#include <helpers.hpp>
#include <constants.hpp>
#include <components.hpp>
#include <scene/PlanetAssault.hpp>
#include <scene/SolarSystem.hpp>

using namespace nongravitar;
using namespace nongravitar::tags;
using namespace nongravitar::scene;
using namespace nongravitar::assets;
using namespace nongravitar::messages;
using namespace nongravitar::constants;
using namespace nongravitar::components;

using helpers::RandomDevice;
using helpers::RandomEngine;
using helpers::IntDistribution;
using helpers::FloatDistribution;

constexpr auto PLANET_MIN_RADIUS = 32.0f;
constexpr auto PLANET_MAX_RADIUS = 64.0f;
constexpr auto PLANET_MAX_DIAMETER = PLANET_MAX_RADIUS * 2.0f;
constexpr auto SPAWN_RADIUS = 64.0f;

SolarSystem::SolarSystem(const SceneId leaderBoardSceneId) :
        mBuffer{}, mRandomEngine{RandomDevice()()}, mLeaderBoardSceneId{leaderBoardSceneId} {}

void SolarSystem::addPlanet(const sf::RenderWindow &window, sf::Color planetColor, SceneId planetSceneId) {
    const auto[windowWidth, windowHeight] = window.getSize();
    const auto spawnPosition = sf::Vector2f(windowWidth, windowHeight) / 2.0f;

    auto planetRadiusDistribution = FloatDistribution(PLANET_MIN_RADIUS, PLANET_MAX_RADIUS);
    auto planetXDistribution = FloatDistribution(PLANET_MAX_DIAMETER, windowWidth - PLANET_MAX_DIAMETER);
    auto planetYDistribution = FloatDistribution(PLANET_MAX_DIAMETER, windowHeight - PLANET_MAX_DIAMETER);

    auto circleShape = sf::CircleShape();
    auto collides = true;

    for (auto i = 0u; collides and i < 128u; i++) {
        collides = false;
        circleShape.setRadius(planetRadiusDistribution(mRandomEngine));
        helpers::centerOrigin(circleShape, circleShape.getLocalBounds());
        circleShape.setPosition(planetXDistribution(mRandomEngine), planetYDistribution(mRandomEngine));

        // if planet collides with spawn circle then retry
        if (helpers::magnitude(spawnPosition, circleShape.getPosition()) <= SPAWN_RADIUS + circleShape.getRadius()) {
            collides = true;
            continue;
        }

        // if planet collides with other entities then retry
        const auto view = mRegistry.view<Renderable, HitRadius>();
        for (const auto entityId : view) {
            const auto &[entityRenderable, entityHitRadius] = view.get<Renderable, HitRadius>(entityId);

            if (helpers::magnitude(entityRenderable->getPosition(), circleShape.getPosition()) <= *entityHitRadius + circleShape.getRadius()) {
                collides = true;
                break;
            }
        }
    }

    if (collides) {
        std::cerr << trace("Unable to generate a random planet") << std::endl;
        std::terminate();
    } else {
        const auto planetId = mRegistry.create();

        circleShape.setFillColor(planetColor);
        mRegistry.assign<Planet>(planetId);
        mRegistry.assign<SceneRef>(planetId, planetSceneId);
        mRegistry.assign<HitRadius>(planetId, circleShape.getRadius());
        mRegistry.assign<Renderable>(planetId, std::move(circleShape));
    }
}

SceneId SolarSystem::update(const sf::RenderWindow &window, SceneManager &sceneManager, Assets &assets, const sf::Time elapsed) {
    mNextSceneId = getSceneId();

    if (auto &audioManager = assets.getAudioManager(); SoundTrackId::ComputerF__k != audioManager.getPlaying()) {
        audioManager.play(SoundTrackId::ComputerF__k);
    }

    livenessSystem(window, sceneManager, assets);
    inputSystem(elapsed);
    motionSystem(elapsed);
    collisionSystem(window);
    reportSystem(window);

    return mNextSceneId;
}

void SolarSystem::render(sf::RenderTarget &window) const {
    window.draw(mReport);

    mRegistry.view<const Renderable>().each([&](const auto id, const auto &renderable) {
        helpers::debug([&]() { // display hit-circle on debug builds only
            if (const auto hitRadius = mRegistry.try_get<HitRadius>(id); hitRadius) {
                auto shape = sf::CircleShape(**hitRadius);
                helpers::centerOrigin(shape, shape.getLocalBounds());
                shape.setPosition(renderable->getPosition());
                shape.setFillColor(sf::Color::Transparent);
                shape.setOutlineColor(sf::Color::Red);
                shape.setOutlineThickness(1);
                window.draw(shape);
            }
        });

        window.draw(renderable);
    });
}

Scene &SolarSystem::setup(const sf::RenderWindow &window, Assets &assets) {
    mReport.setFont(assets.getFontsManager().getFont(FontId::Mechanical));
    mReport.setFillColor(sf::Color(105, 235, 245, 255));
    mReport.setCharacterSize(18.0f);

    initializePlayers(window, assets);

    pubsub::subscribe<messages::SolarSystemEntered>(*this);
    return Scene::setup(window, assets);
}

void SolarSystem::operator()(const SolarSystemEntered &message) {
    const auto planets = mRegistry.view<Planet, SceneRef>();

    for (const auto planetId : planets) {
        if (message.sceneId == *planets.get<SceneRef>(planetId)) {
            const auto players = mRegistry.view<Player>();

            mRegistry.destroy(players.begin(), players.end());
            for (const auto sourcePlayerId : message.registry.view<Player>()) {
                const auto playerId = mRegistry.create(sourcePlayerId, message.registry);
                mRegistry.get<Renderable>(playerId)->setPosition(sf::Vector2f(message.window.getSize()) / 2.0f);
                mRegistry.remove<EntityRef<Tractor>>(playerId);
            }

            if (const auto bunkers = message.registry.view<Bunker>(); bunkers.begin() == bunkers.end()) {
                mRegistry.view<Player, Score>().each([&](const auto, auto &score) { score.value += message.bonus; });
                mRegistry.destroy(planetId);
            }

            return; // we can exit the function because we found our planet and we know that each planet has a unique id.
        }
    }
}

void SolarSystem::initializePlayers(const sf::RenderWindow &window, Assets &assets) {
    auto playerId = mRegistry.create();
    auto playerRenderable = assets.getSpriteSheetsManager().getSpriteSheet(SpriteSheetId::SpaceShip).instanceSprite(0);
    const auto playerBounds = playerRenderable.getLocalBounds();

    helpers::centerOrigin(playerRenderable, playerBounds);
    playerRenderable.setPosition(sf::Vector2f(window.getSize()) / 2.0f);
    playerRenderable.setRotation(90.0f);

    mRegistry.assign<Player>(playerId);
    mRegistry.assign<Score>(playerId);
    mRegistry.assign<Damage>(playerId, 1);
    mRegistry.assign<Health>(playerId, PLAYER_HEALTH);
    mRegistry.assign<Energy>(playerId, PLAYER_ENERGY);
    mRegistry.assign<Velocity>(playerId);
    mRegistry.assign<ReloadTime>(playerId, PLAYER_RELOAD_TIME);
    mRegistry.assign<HitRadius>(playerId, std::max(playerBounds.width, playerBounds.height) / 2.0f);
    mRegistry.assign<Renderable>(playerId, std::move(playerRenderable));
}

void SolarSystem::initializePlanets(const sf::RenderWindow &window, SceneManager &sceneManager, Assets &assets) {
    const auto windowCenter = sf::Vector2f(window.getSize()) / 2.0f;
    auto planetsColorsSelector = IntDistribution(0, PLANET_COLORS.size() - 1);

    mRegistry.view<Player, Renderable>().each([&](const auto, auto &renderable) {
        renderable->setPosition(windowCenter);
    });

    const auto solarSystemSceneId = getSceneId();
    for (auto i = 0u; i < PLANETS; i++) {
        const auto rgb = PLANET_COLORS[planetsColorsSelector(mRandomEngine)];
        const auto planetColor = sf::Color(rgb[0], rgb[1], rgb[2]);
        const auto &planetAssault = sceneManager.emplace<PlanetAssault>(window, assets, solarSystemSceneId, mLeaderBoardSceneId, planetColor);
        addPlanet(window, planetColor, planetAssault.getSceneId());
    }
}

void SolarSystem::inputSystem(const sf::Time elapsed) {
    using Key = sf::Keyboard::Key;
    const auto isKeyPressed = &sf::Keyboard::isKeyPressed;

    mRegistry
            .view<Player, Energy, Velocity, Renderable>()
            .each([&](const auto, auto &playerEnergy, auto &playerVelocity, auto &playerRenderable) {
                auto speed = PLAYER_SPEED;

                if (isKeyPressed(Key::W)) {
                    speed *= 1.32f;
                } else if (isKeyPressed(Key::S)) {
                    speed *= 0.88f;
                }

                if (isKeyPressed(Key::A)) {
                    playerRenderable->rotate(-PLAYER_ROTATION_SPEED * elapsed.asSeconds());
                }

                if (isKeyPressed(Key::D)) {
                    playerRenderable->rotate(PLAYER_ROTATION_SPEED * elapsed.asSeconds());
                }

                playerVelocity.value = helpers::makeVector2(playerRenderable->getRotation(), speed);
                playerEnergy.consume(speed * elapsed.asSeconds());
            });
}

void SolarSystem::motionSystem(const sf::Time elapsed) {
    mRegistry.view<Velocity, Renderable>().each([&](const auto &velocity, auto &renderable) {
        renderable->move(velocity.value * elapsed.asSeconds());
    });
}

void SolarSystem::collisionSystem(const sf::RenderWindow &window) {
    const auto viewport = sf::FloatRect(window.getViewport(window.getView()));
    const auto players = mRegistry.view<Player, HitRadius, Renderable>();

    for (const auto playerId : players) {
        const auto &[playerHitRadius, playerRenderable] = players.get<HitRadius, Renderable>(playerId);

        if (viewport.contains(playerRenderable->getPosition())) {
            const auto planets = mRegistry.view<Planet, Renderable, HitRadius, SceneRef>();

            for (const auto planetId : planets) {
                const auto &[planetHitRadius, planetRenderable, planetSceneRef] = planets.get<HitRadius, Renderable, SceneRef>(planetId);

                if (helpers::magnitude(playerRenderable->getPosition(), planetRenderable->getPosition()) <= *playerHitRadius + *planetHitRadius) {
                    mNextSceneId = *planetSceneRef;
                    pubsub::publish<PlanetEntered>(window, mRegistry, *planetSceneRef);
                    return; // we can enter only one planet at a time
                }
            }
        } else {
            auto[playerX, playerY] = playerRenderable->getPosition();

            if (playerX <= 0) {
                playerX = viewport.width - *playerHitRadius;
            } else if (playerX >= viewport.width) {
                playerX = *playerHitRadius;
            }

            if (playerY <= 0) {
                playerY = viewport.height - *playerHitRadius;
            } else if (playerY >= viewport.height) {
                playerY = *playerHitRadius;
            }

            playerRenderable->setPosition(playerX, playerY);
        }
    }
}

void SolarSystem::livenessSystem(const sf::RenderWindow &window, SceneManager &sceneManager, Assets &assets) {
    auto entitiesToDestroy = std::vector<entt::entity>();

    const auto players = mRegistry.view<Player, Health, Energy>();
    for (const auto id : players) {
        const auto &[health, energy] = players.get<Health, Energy>(id);
        if (health.isOver() or energy.isOver()) {
            assets.getAudioManager().play(SoundId::Explosion);
            entitiesToDestroy.push_back(id);
            mNextSceneId = mLeaderBoardSceneId;
            pubsub::publish<GameOver>(mRegistry.get<Score>(id).value);
            return;
        }
    }

    if (mRegistry.view<Planet>().begin() == mRegistry.view<Planet>().end()) { // no more planets left
        initializePlanets(window, sceneManager, assets);
    }

    mRegistry.destroy(entitiesToDestroy.begin(), entitiesToDestroy.end());
}

void SolarSystem::reportSystem(const sf::RenderWindow &window) {
    mRegistry.view<Player, Health, Energy, Score>().each([&](const auto, const auto &health, const auto &energy, const auto &score) {
        std::snprintf(
                mBuffer, std::size(mBuffer),
                "health: %02d energy: %05.0f score: %05u",
                health.getValue(), energy.getValue(), score.value
        );
        helpers::centerOrigin(mReport, mReport.getLocalBounds());

        mReport.setString(mBuffer);
        mReport.setPosition(window.getSize().x / 2.0f, 18.0f);
    });
}
