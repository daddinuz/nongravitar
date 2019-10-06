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
#include <scene/SolarSystem.hpp>

using namespace gravitar;
using namespace gravitar::tags;
using namespace gravitar::scene;
using namespace gravitar::assets;
using namespace gravitar::messages;
using namespace gravitar::constants;
using namespace gravitar::components;

using RandomEngine = helpers::RandomEngine;
using ByteDistribution = helpers::ByteDistribution;
using FloatDistribution = helpers::FloatDistribution;

SolarSystem::SolarSystem(const SceneId youWonSceneId, const SceneId gameOverSceneId) :
        mBuffer{},
        mYouWonSceneId{youWonSceneId},
        mGameOverSceneId{gameOverSceneId} {}

SolarSystem &SolarSystem::initialize(const sf::RenderWindow &window, Assets &assets) noexcept {
    initializePlayers(window, assets);
    initializeReport(assets);
    initializePubSub();
    return *this;
}

void SolarSystem::addPlanet(const SceneId planetSceneId, const sf::RenderWindow &window, RandomEngine &randomEngine) noexcept {
    const auto[windowWidth, windowHeight] = window.getSize();
    auto planetXDistribution = FloatDistribution(0.0f, windowWidth);
    auto planetYDistribution = FloatDistribution(0.0f, windowHeight);
    auto planetSizeDistribution = FloatDistribution(24, 56);
    const auto planetId = mRegistry.create();
    auto &planetRenderable = mRegistry.assign<Renderable>(planetId, sf::CircleShape());
    mRegistry.assign<SceneRef>(planetId, planetSceneId);
    mRegistry.assign<Planet>(planetId);

    auto collides = true;
    for (auto i = 0; collides and i < 128; i++) {
        collides = false;

        auto &circleShape = planetRenderable.as<sf::CircleShape>();
        circleShape.setRadius(planetSizeDistribution(randomEngine));
        helpers::centerOrigin(*planetRenderable, circleShape.getLocalBounds());
        planetRenderable->setPosition(planetXDistribution(randomEngine), planetYDistribution(randomEngine));

        auto &planetHitRadius = mRegistry.assign_or_replace<HitRadius>(planetId, circleShape.getRadius());

        // if planet collides with other entities then retry
        const auto view = mRegistry.view<HitRadius, Renderable>();
        for (const auto entityId : view) {
            if (planetId != entityId) {
                const auto &[entityHitRadius, entityRenderable] = view.get<HitRadius, Renderable>(entityId);

                if (helpers::magnitude(entityRenderable->getPosition(), planetRenderable->getPosition()) <= *planetHitRadius + *entityHitRadius) {
                    collides = true;
                    break;
                }
            }
        }
    }

    if (collides) {
        std::cerr << trace("Unable to generate a random planet") << std::endl;
        std::terminate();
    } else {
        auto &circleShape = planetRenderable.as<sf::CircleShape>();

        circleShape.setFillColor(sf::Color(
                helpers::ByteDistribution(63, 255)(randomEngine),
                helpers::ByteDistribution(63, 255)(randomEngine),
                helpers::ByteDistribution(63, 255)(randomEngine),
                helpers::ByteDistribution(63, 199)(randomEngine)
        ));
        circleShape.setOutlineColor(sf::Color(
                helpers::ByteDistribution(31, 127)(randomEngine),
                helpers::ByteDistribution(31, 127)(randomEngine),
                helpers::ByteDistribution(31, 127)(randomEngine),
                helpers::ByteDistribution(63, 127)(randomEngine)
        ));
        circleShape.setOutlineThickness(helpers::FloatDistribution(4, 8)(randomEngine));
    }
}

SceneId SolarSystem::update(const sf::RenderWindow &window, Assets &assets, const sf::Time elapsed) noexcept {
    (void) assets;
    mNextSceneId = getSceneId();

    if (auto &audioManager = assets.getAudioManager(); SoundTrackId::ComputerF__k != audioManager.getPlaying()) {
        audioManager.play(SoundTrackId::ComputerF__k);
    }

    inputSystem(window, elapsed);
    motionSystem(elapsed);
    collisionSystem(window);
    livenessSystem();
    reportSystem(window);

    return mNextSceneId;
}

void SolarSystem::render(sf::RenderTarget &window) noexcept {
    window.draw(mReport);

    mRegistry.view<Renderable>().each([&](const auto id, const auto &renderable) {
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

void SolarSystem::operator()(const PlanetDestroyed &message) noexcept {
    mRegistry.view<Planet, SceneRef>().each([&](const auto id, const auto tag, const auto &sceneRef) {
        (void) tag;

        if (message.sceneId == *sceneRef) {
            mRegistry.destroy(id);
        }
    });
}

void SolarSystem::operator()(const SolarSystemEntered &message) noexcept {
    mRegistry.view<Planet, SceneRef>().each([&](const auto tag, const auto &sceneRef) {
        (void) tag;

        if (message.sceneId == *sceneRef) {
            const auto players = mRegistry.view<Player>();

            mRegistry.destroy(players.begin(), players.end());
            for (const auto sourcePlayerId : message.sourceRegistry.view<Player>()) {
                const auto playerId = mRegistry.create(sourcePlayerId, message.sourceRegistry);
                mRegistry.remove<EntityRef<Tractor>>(playerId);
            }
        }
    });
}

void SolarSystem::initializePubSub() const noexcept {
    pubsub::subscribe<messages::SolarSystemEntered>(*this);
    pubsub::subscribe<messages::PlanetDestroyed>(*this);
}

void SolarSystem::initializeReport(Assets &assets) noexcept {
    mReport.setCharacterSize(18);
    mReport.setFillColor(sf::Color(105, 235, 245, 255));
    mReport.setFont(assets.getFontsManager().get(FontId::Mechanical));
}

void SolarSystem::initializePlayers(const sf::RenderWindow &window, Assets &assets) noexcept {
    auto playerId = mRegistry.create();
    auto playerRenderable = assets.getSpriteSheetsManager().get(SpriteSheetId::SpaceShip).instanceSprite(0);
    const auto playerBounds = playerRenderable.getLocalBounds();

    helpers::centerOrigin(playerRenderable, playerBounds);
    playerRenderable.setPosition(sf::Vector2f(window.getSize()) / 2.0f);

    mRegistry.assign<Player>(playerId);
    mRegistry.assign<Health>(playerId, PLAYER_HEALTH);
    mRegistry.assign<Fuel>(playerId, PLAYER_FUEL);
    mRegistry.assign<Velocity>(playerId);
    mRegistry.assign<ReloadTime>(playerId, PLAYER_RELOAD_TIME);
    mRegistry.assign<HitRadius>(playerId, std::max(playerBounds.width, playerBounds.height) / 2.0f);
    mRegistry.assign<Renderable>(playerId, std::move(playerRenderable));
}

void SolarSystem::inputSystem(const sf::RenderWindow &window, const sf::Time elapsed) noexcept {
    using Key = sf::Keyboard::Key;
    const auto keyPressed = &sf::Keyboard::isKeyPressed;

    mRegistry.view<Player, Fuel, Velocity, Renderable>().each([&](const auto tag, auto &fuel, auto &velocity, auto &renderable) {
        (void) tag;

        auto speed = PLAYER_SPEED;
        const auto input = (keyPressed(Key::A) ? 1 : 0) + (keyPressed(Key::D) ? 2 : 0) +
                           (keyPressed(Key::W) ? 4 : 0) + (keyPressed(Key::S) ? 8 : 0);

        switch (input) {
            case 1:
                renderable->rotate(-PLAYER_ROTATION_SPEED * elapsed.asSeconds());
                break;

            case 2:
                renderable->rotate(PLAYER_ROTATION_SPEED * elapsed.asSeconds());
                break;

            case 4:
                speed *= 1.56f;
                break;

            case 5:
                renderable->rotate(-PLAYER_ROTATION_SPEED * 0.92f * elapsed.asSeconds());
                speed *= 1.32f;
                break;

            case 6:
                renderable->rotate(PLAYER_ROTATION_SPEED * 0.92f * elapsed.asSeconds());
                speed *= 1.32f;
                break;

            case 8:
                speed *= 0.98f;
                break;

            case 9:
                renderable->rotate(-PLAYER_ROTATION_SPEED * 1.08f * elapsed.asSeconds());
                speed *= 0.68f;
                break;

            case 10:
                renderable->rotate(PLAYER_ROTATION_SPEED * 1.08f * elapsed.asSeconds());
                speed *= 0.68f;
                break;

            default: {
                const auto mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                const auto mouseRotation = helpers::rotation(renderable->getPosition(), mousePosition);
                const auto shortestRotation = helpers::shortestRotation(renderable->getRotation(), mouseRotation);
                renderable->rotate(static_cast<float>(helpers::signum(shortestRotation)) * PLAYER_ROTATION_SPEED * elapsed.asSeconds());
            }
        }

        velocity.value = helpers::makeVector2(renderable->getRotation(), speed);
        fuel.value -= speed * elapsed.asSeconds();
    });
}

void SolarSystem::motionSystem(const sf::Time elapsed) noexcept {
    mRegistry.view<Velocity, Renderable>().each([&](const auto &velocity, auto &renderable) {
        renderable->move(velocity.value * elapsed.asSeconds());
    });
}

void SolarSystem::collisionSystem(const sf::RenderWindow &window) noexcept {
    const auto viewport = sf::FloatRect(window.getViewport(window.getView()));

    mRegistry
            .view<Player, HitRadius, Renderable>()
            .each([&](const auto playerId, const auto playerTag, const auto &playerHitRadius, auto &playerRenderable) {
                (void) playerTag;

                if (viewport.contains(playerRenderable->getPosition())) {
                    const auto planets = mRegistry.view<Planet, Renderable, HitRadius, SceneRef>();

                    for (const auto planetId : planets) {
                        const auto &[planetRenderable, planetHitRadius, planetSceneRef] = planets.get<Renderable, HitRadius, SceneRef>(planetId);

                        if (helpers::magnitude(playerRenderable->getPosition(), planetRenderable->getPosition()) <= *playerHitRadius + *planetHitRadius) {
                            mNextSceneId = *planetSceneRef;
                            planetRenderable->setRotation(90.0f);
                            playerRenderable->setPosition(window.getSize().x / 2.0f, 128.0f);
                            pubsub::publish<PlanetEntered>(*planetSceneRef, mRegistry);
                            break; // we can enter only one planet at a time
                        }
                    }
                } else {
                    mRegistry.get<Health>(playerId).value -= 1;
                    playerRenderable->setPosition(sf::Vector2f(window.getSize()) / 2.0f);
                }
            });
}

void SolarSystem::livenessSystem() noexcept {
    if (mRegistry.view<Planet>().begin() == mRegistry.view<Planet>().end()) { // no more planets left
        mNextSceneId = mYouWonSceneId;
    }

    mRegistry.view<Player, Health, Fuel>().each([&](const auto id, const auto tag, const auto &health, const auto &fuel) {
        (void) tag;

        if (health.isDead() or fuel.isOver()) {
            mRegistry.destroy(id);
            mNextSceneId = mGameOverSceneId;
        }
    });
}

void SolarSystem::reportSystem(const sf::RenderWindow &window) noexcept {
    mRegistry.view<Player, Health, Fuel>().each([&](const auto tag, const auto &health, const auto &fuel) {
        (void) tag;

        std::snprintf(mBuffer, std::size(mBuffer), "health: %d fuel: %.0f", health.value, fuel.value);
        helpers::centerOrigin(mReport, mReport.getLocalBounds());

        mReport.setString(mBuffer);
        mReport.setPosition(window.getSize().x / 2.0f, 18.0f);
    });
}
