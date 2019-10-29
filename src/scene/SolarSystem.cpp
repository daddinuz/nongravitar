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
#include <Canvas.hpp>
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

constexpr auto WINDOW_PADDING = 64u;
constexpr auto SPAWN_AREA_RADIUS = 64.0f;

SolarSystem::SolarSystem(const SceneId leaderBoardSceneId) :
        mBuffer{}, mRandomEngine{RandomDevice()()}, mLeaderBoardSceneId{leaderBoardSceneId} {}

void SolarSystem::addPlanet(const sf::RenderWindow &window, Assets &assets, sf::Color planetColor, SceneId planetSceneId) {
    const auto[windowWidth, windowHeight] = sf::Vector2f(window.getSize());
    const auto windowCenter = sf::Vector2f(windowWidth / 2.0f, windowHeight / 2.0f);

    auto planetScaleDistribution = FloatDistribution(0.5f, 1.0f);
    auto planetXDistribution = FloatDistribution(WINDOW_PADDING, windowWidth - WINDOW_PADDING);
    auto planetYDistribution = FloatDistribution(WINDOW_PADDING, windowHeight - WINDOW_PADDING);

    const auto planetId = mRegistry.create();
    const auto &planetSprite = mRegistry.assign<Sprite>(planetId, assets.getSpriteSheetsManager().getSprite(SpriteSheetId::Planet, 0));
    auto &planetTransformation = mRegistry.assign<Transformation>(planetId);

    planetTransformation.setOrigin(sf::Vector2f(planetSprite.getSize()) / 2.0f);

    mRegistry.assign<Planet>(planetId);
    mRegistry.assign<Color>(planetId, planetColor);
    mRegistry.assign<SceneRef>(planetId, planetSceneId);

    auto collides = true;
    for (auto i = 0u; collides and i < 128u; i++) {
        const auto scaleFactor = planetScaleDistribution(mRandomEngine);
        const auto &planetHitRadius = mRegistry.assign_or_replace<HitRadius>(planetId, planetSprite.getRadius() * scaleFactor);

        collides = false;
        planetTransformation.setScale(scaleFactor, scaleFactor);
        planetTransformation.setPosition(planetXDistribution(mRandomEngine), planetYDistribution(mRandomEngine));

        // if planet collides with spawn area then retry
        if (helpers::magnitude(windowCenter, planetTransformation.getPosition()) <= SPAWN_AREA_RADIUS + *planetHitRadius) {
            collides = true;
            continue;
        }

        // if planet collides with other entities then retry
        const auto entities = mRegistry.view<Transformation, HitRadius>();
        for (const auto entityId : entities) {
            if (entityId != planetId) {
                const auto &[entityTransformation, entityHitRadius] = entities.get<Transformation, HitRadius>(entityId);

                if (helpers::magnitude(entityTransformation.getPosition(), planetTransformation.getPosition()) <= *entityHitRadius + *planetHitRadius) {
                    collides = true;
                    break;
                }
            }
        }
    }

    if (collides) {
        std::cerr << trace("Unable to generate a random planet") << std::endl;
        std::terminate();
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
    auto canvas = Canvas();

    mRegistry
            .view<const Transformation, const Sprite>()
            .each([&](const auto id, const auto &transformable, const auto &sprite) {
                canvas.bind(sprite);

                if (const auto color = mRegistry.try_get<Color>(id); color) {
                    canvas.setColor(*color);
                } else {
                    canvas.setColor(sf::Color::White);
                }

                window.draw(canvas, transformable.getTransform());
            });

    helpers::debug([&]() { // display hit-circle on debug builds only
        mRegistry
                .view<const Transformation, const HitRadius>()
                .each([&](const auto &transformable, const auto &hitRadius) {
                    auto shape = sf::CircleShape(*hitRadius);
                    helpers::centerOrigin(shape);
                    shape.setPosition(transformable.getPosition());
                    shape.setFillColor(sf::Color::Transparent);
                    shape.setOutlineColor(sf::Color::Red);
                    shape.setOutlineThickness(1);
                    window.draw(shape);
                });
    });

    window.draw(mReport);
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
                mRegistry.get<Transformation>(playerId).setPosition(sf::Vector2f(message.window.getSize()) / 2.0f);
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
    const auto playerId = mRegistry.create();
    const auto &playerSprite = mRegistry.assign<Sprite>(playerId, assets.getSpriteSheetsManager().getSprite(SpriteSheetId::SpaceShip, 0));

    auto &playerTransformation = mRegistry.assign<Transformation>(playerId);
    playerTransformation.setOrigin(sf::Vector2f(playerSprite.getSize()) / 2.0f);
    playerTransformation.setPosition(sf::Vector2f(window.getSize()) / 2.0f);
    playerTransformation.setRotation(90.0f);

    mRegistry.assign<Player>(playerId);
    mRegistry.assign<Score>(playerId);
    mRegistry.assign<Damage>(playerId, 1);
    mRegistry.assign<Health>(playerId, PLAYER_HEALTH);
    mRegistry.assign<Energy>(playerId, PLAYER_ENERGY);
    mRegistry.assign<Velocity>(playerId);
    mRegistry.assign<ReloadTime>(playerId, PLAYER_RELOAD_TIME);
    mRegistry.assign<HitRadius>(playerId, playerSprite.getRadius());
}

void SolarSystem::initializePlanets(const sf::RenderWindow &window, SceneManager &sceneManager, Assets &assets) {
    const auto windowCenter = sf::Vector2f(window.getSize()) / 2.0f;
    auto planetsColorsSelector = IntDistribution(0, PLANET_COLORS.size() - 1);

    mRegistry.view<Player, Transformation>().each([&](const auto, auto &transformable) {
        transformable.setPosition(windowCenter);
    });

    const auto solarSystemSceneId = getSceneId();
    for (auto i = 0u; i < PLANETS; i++) {
        const auto rgb = PLANET_COLORS[planetsColorsSelector(mRandomEngine)];
        const auto planetColor = sf::Color(rgb[0], rgb[1], rgb[2]);
        const auto &planetAssault = sceneManager.emplace<PlanetAssault>(window, assets, solarSystemSceneId, mLeaderBoardSceneId, planetColor);
        addPlanet(window, assets, planetColor, planetAssault.getSceneId());
    }
}

void SolarSystem::inputSystem(const sf::Time elapsed) {
    using Key = sf::Keyboard::Key;
    const auto isKeyPressed = &sf::Keyboard::isKeyPressed;

    mRegistry
            .view<Player, Transformation, Velocity, Energy>()
            .each([&](const auto, auto &transformable, auto &velocity, auto &energy) {
                const auto speed = isKeyPressed(Key::W) ? PLAYER_SPEED_FAST : isKeyPressed(Key::S) ? PLAYER_SPEED_SLOW : PLAYER_SPEED_DEFAULT;
                const auto rotationSign = isKeyPressed(Key::A) ? -1.0f : isKeyPressed(Key::D) ? 1.0f : 0.0f;

                transformable.rotate(rotationSign * PLAYER_ROTATION_SPEED * elapsed.asSeconds());
                velocity.value = helpers::makeVector2(transformable.getRotation(), speed);
                energy.consume(speed * elapsed.asSeconds());
            });
}

void SolarSystem::motionSystem(const sf::Time elapsed) {
    mRegistry.view<Transformation, Velocity>().each([&](auto &transformable, const auto &velocity) {
        transformable.move(velocity.value * elapsed.asSeconds());
    });
}

void SolarSystem::collisionSystem(const sf::RenderWindow &window) {
    const auto viewport = sf::FloatRect(window.getViewport(window.getView()));
    const auto players = mRegistry.view<Player, Transformation, HitRadius>();

    for (const auto playerId : players) {
        const auto &[playerTransformation, playerHitRadius] = players.get<Transformation, HitRadius>(playerId);

        if (viewport.contains(playerTransformation.getPosition())) {
            const auto planets = mRegistry.view<Planet, Transformation, HitRadius, SceneRef>();

            for (const auto planetId : planets) {
                const auto &[planetTransformation, planetHitRadius, planetSceneRef] = planets.get<Transformation, HitRadius, SceneRef>(planetId);

                if (helpers::magnitude(playerTransformation.getPosition(), planetTransformation.getPosition()) <= *playerHitRadius + *planetHitRadius) {
                    mNextSceneId = *planetSceneRef;
                    pubsub::publish<PlanetEntered>(window, mRegistry, *planetSceneRef);
                    return; // we can enter only one planet at a time
                }
            }
        } else {
            auto[playerX, playerY] = playerTransformation.getPosition();

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

            playerTransformation.setPosition(playerX, playerY);
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
        helpers::centerOrigin(mReport);
        mReport.setString(mBuffer);
        mReport.setPosition(window.getSize().x / 2.0f, 18.0f);
    });
}
