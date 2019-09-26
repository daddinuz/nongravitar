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
#include <components.hpp>
#include <scene/SolarSystem.hpp>

using namespace gravitar;
using namespace gravitar::tags;
using namespace gravitar::scene;
using namespace gravitar::assets;
using namespace gravitar::messages;
using namespace gravitar::components;

constexpr float SPEED = 180.0f;
constexpr float ROTATION_SPEED = 180.0f;

SolarSystem::SolarSystem(const SceneId youWonSceneId, const SceneId gameOverSceneId, Assets &assets) :
        mBuffer{},
        mYouWonSceneId{youWonSceneId},
        mGameOverSceneId{gameOverSceneId} {
    mReport.setCharacterSize(18);
    mReport.setFillColor(sf::Color(105, 235, 245, 255));
    mReport.setFont(assets.getFontsManager().get(FontId::Mechanical));

    mRegistry.group<Renderable, Velocity>();
    mRegistry.group<Player>(entt::get < Renderable > );
    mRegistry.group<Planet, SceneSwitcher>(entt::get < Renderable > );
    mRegistry.group<Bullet>(entt::get < Renderable > );

    mRegistry.group<>(entt::get < Player, Health, Fuel > );
}

void SolarSystem::operator()(const PlanetExited &planetExited) noexcept {
    mRegistry.group<Planet, SceneSwitcher>(entt::get < Renderable > ).each([&](const auto &planetTag, const auto &sceneSwitcher, const auto &planetRenderable) {
        (void) planetTag;
        (void) planetRenderable;

        if (planetExited.planetSceneId == sceneSwitcher.getSceneId()) {
            const auto players = mRegistry.view<Player>();
            mRegistry.destroy(players.begin(), players.end());
            mRegistry.create<Player, Fuel, Health, Velocity, RechargeTime, Renderable>(planetExited.playerId, planetExited.sourceRegistry);
        }
    });
}

void SolarSystem::operator()(const PlanetDestroyed &planetDestroyed) noexcept {
    mRegistry.group<Planet, SceneSwitcher>(entt::get < Renderable > ).each([&](const auto planetId, const auto planetTag, const auto &sceneSwitcher, const auto &planetRenderable) {
        (void) planetTag;
        (void) planetRenderable;

        if (planetDestroyed.planetSceneId == sceneSwitcher.getSceneId()) {
            mRegistry.destroy(planetId);
        }
    });
}

SceneId SolarSystem::update(const sf::RenderWindow &window, Assets &assets, const sf::Time elapsed) noexcept {
    (void) assets;
    mNextSceneId = getSceneId();

    inputSystem(window, elapsed);
    motionSystem(elapsed);
    collisionSystem(window);
    livenessSystem();
    reportSystem(window);

    return mNextSceneId;
}

void SolarSystem::render(sf::RenderTarget &window) noexcept {
    window.draw(mReport);

    mRegistry.view<Renderable>().each([&](const auto &renderable) {
        helpers::debug([&]() { // display hit-box on debug builds only
            const auto hitBox = renderable.getHitBox();
            auto shape = sf::RectangleShape({hitBox.width, hitBox.height});
            shape.setPosition({hitBox.left, hitBox.top});
            shape.setFillColor(sf::Color::Transparent);
            shape.setOutlineColor(sf::Color::Red);
            shape.setOutlineThickness(1);
            window.draw(shape);
        });

        window.draw(renderable);
    });
}

void SolarSystem::inputSystem(const sf::RenderWindow &window, const sf::Time elapsed) noexcept {
    mRegistry.group<Player>(entt::get < Renderable > ).each([&](const auto playerId, const auto playerTag, auto &renderable) {
        (void) playerTag;

        auto speed = SPEED;
        const auto &[fuel, velocity] = mRegistry.get<Fuel, Velocity>(playerId);
        const auto input = (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ? 1 : 0) + (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ? 2 : 0) +
                           (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ? 4 : 0) + (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ? 8 : 0);

        switch (input) {
            case 0: {
                const auto mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                const auto mouseRotation = helpers::rotation(renderable.getPosition(), mousePosition);
                const auto shortestRotation = helpers::shortestRotation(renderable.getRotation(), mouseRotation);
                renderable.rotate(static_cast<float>(helpers::signum(shortestRotation)) * ROTATION_SPEED * elapsed.asSeconds());
            }
                break;

            case 1:
                renderable.rotate(-ROTATION_SPEED * elapsed.asSeconds());
                break;

            case 2:
                renderable.rotate(ROTATION_SPEED * elapsed.asSeconds());
                break;

            case 4:
                speed *= 1.56f;
                break;

            case 5:
                renderable.rotate(-ROTATION_SPEED * 0.92f * elapsed.asSeconds());
                speed *= 1.32f;
                break;

            case 6:
                renderable.rotate(ROTATION_SPEED * 0.92f * elapsed.asSeconds());
                speed *= 1.32f;
                break;

            case 8:
                speed *= 0.98f;
                break;

            case 9:
                renderable.rotate(-ROTATION_SPEED * 1.08f * elapsed.asSeconds());
                speed *= 0.68f;
                break;

            case 10:
                renderable.rotate(ROTATION_SPEED * 1.08f * elapsed.asSeconds());
                speed *= 0.68f;
                break;

            default:
                break;
        }

        velocity.value = helpers::makeVector2(renderable.getRotation(), speed);
        fuel.value -= speed * elapsed.asSeconds();
    });
}

void SolarSystem::motionSystem(const sf::Time elapsed) noexcept {
    mRegistry.group<Renderable, Velocity>().each([&](auto &renderable, const auto &velocity) {
        renderable.move(velocity.value * elapsed.asSeconds());
    });
}

void SolarSystem::collisionSystem(const sf::RenderWindow &window) noexcept {
    const auto viewport = sf::FloatRect(window.getViewport(window.getView()));

    mRegistry.group<Player>(entt::get < Renderable > ).each([&](const auto playerId, const auto playerTag, auto &playerRenderable) {
        (void) playerTag;

        if (not playerRenderable.getHitBox().intersects(viewport)) { // player-window intersection
            mRegistry.get<Health>(playerId).value -= 1;
            playerRenderable.setPosition(sf::Vector2f(window.getSize()) / 2.0f);
        } else { // player-planet intersection
            const auto group = mRegistry.group<Planet, SceneSwitcher>(entt::get < Renderable > );
            for (const auto planetId : group) {
                const auto &[sceneSwitcher, planetRenderable] = group.get<SceneSwitcher, Renderable>(planetId);

                if (playerRenderable.getHitBox().intersects(planetRenderable.getHitBox())) {
                    mNextSceneId = sceneSwitcher.getSceneId();
                    playerRenderable.setPosition(sf::Vector2f(window.getSize()) / 2.0f);
                    pubsub::publish<PlanetEntered>(sceneSwitcher.getSceneId(), playerId, mRegistry);
                    break;
                }
            }
        }
    });
}

void SolarSystem::livenessSystem() noexcept {
    auto livingPlanets = false;

    for (const auto planetId : mRegistry.view<Planet>()) {
        (void) planetId;
        livingPlanets = true;
        break;
    }

    if (not livingPlanets) {
        mNextSceneId = mYouWonSceneId;
    }

    mRegistry.group<>(entt::get < Player, Health, Fuel > ).each([&](const auto playerId, const auto playerTag, const auto &health, const auto &fuel) {
        (void) playerTag;

        if (health.isDead() or fuel.isOver()) {
            mRegistry.destroy(playerId);
            mNextSceneId = mGameOverSceneId;
        }
    });
}

void SolarSystem::reportSystem(const sf::RenderWindow &window) noexcept {
    mRegistry.group<>(entt::get < Player, Health, Fuel > ).each([&](const auto playerTag, const auto &health, const auto &fuel) {
        (void) playerTag;

        std::snprintf(mBuffer, std::size(mBuffer), "health: %d    fuel: %3.0f", health.value, fuel.value);

        mReport.setString(mBuffer);
        helpers::centerOrigin(mReport, mReport.getLocalBounds());

        mReport.setPosition(window.getSize().x / 2.0f, 18.0f);
    });
}

void SolarSystem::addPlayer(const sf::RenderWindow &window, Assets &assets) noexcept {
    auto playerId = mRegistry.create();
    auto renderable = assets.getSpriteSheetsManager().get(SpriteSheetId::SpaceShip).instanceSprite(0);

    helpers::centerOrigin(renderable, renderable.getLocalBounds());
    renderable.setPosition(sf::Vector2f(window.getSize()) / 2.0f);

    mRegistry.assign<Player>(playerId);
    mRegistry.assign<Health>(playerId, 3);
    mRegistry.assign<Fuel>(playerId, 20000.0f);
    mRegistry.assign<Velocity>(playerId);
    mRegistry.assign<RechargeTime>(playerId, RechargeTime(1.0f));
    mRegistry.assign<Renderable>(playerId, std::move(renderable));
}

void SolarSystem::addPlanet(const SceneId sceneId, const sf::RenderWindow &window, std::mt19937 &randomEngine) noexcept {
    using u8_distribution = std::uniform_int_distribution<sf::Uint8>;
    using f32_distribution = std::uniform_real_distribution<float>;

    const auto[windowWidth, windowHeight] = window.getSize();

    auto collides = true;
    auto planetId = mRegistry.create();
    decltype(auto) planetRenderable = mRegistry.assign<Renderable>(planetId, sf::CircleShape());
    decltype(auto) circleShape = std::get<sf::CircleShape>(*planetRenderable);
    mRegistry.assign<SceneSwitcher>(planetId, sceneId);
    mRegistry.assign<Planet>(planetId);

    for (auto i = 0; collides and i < 32; i++) {
        collides = false;

        circleShape.setRadius(f32_distribution(24, 56)(randomEngine));
        circleShape.setPosition(
                f32_distribution(0, windowWidth)(randomEngine),
                f32_distribution(0, windowHeight)(randomEngine)
        );
        helpers::centerOrigin(circleShape, circleShape.getLocalBounds());

        // if planet collides with other entities then retry
        const auto view = mRegistry.view<const Renderable>();
        for (const auto entityId : view) {
            if (planetId != entityId and view.get(entityId).getHitBox().intersects(planetRenderable.getHitBox())) {
                collides = true;
                break;
            }
        }
    }

    if (collides) {
        std::cerr << trace("Unable to generate a random planet") << std::endl;
        std::terminate();
    } else {
        circleShape.setFillColor(sf::Color(
                u8_distribution(63, 255)(randomEngine),
                u8_distribution(63, 255)(randomEngine),
                u8_distribution(63, 255)(randomEngine),
                u8_distribution(63, 199)(randomEngine)
        ));
        circleShape.setOutlineColor(sf::Color(
                u8_distribution(31, 127)(randomEngine),
                u8_distribution(31, 127)(randomEngine),
                u8_distribution(31, 127)(randomEngine),
                u8_distribution(63, 127)(randomEngine)
        ));
        circleShape.setOutlineThickness(f32_distribution(4, 8)(randomEngine));
    }
}
