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

#include <tags.hpp>
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

    mRegistry.group<Planet, SceneSwitcher>();
    mRegistry.group<Renderable, Velocity>();
    mRegistry.group<Health, Fuel>();

    auto player = mRegistry.create();
    auto renderable = assets.getSpriteSheetsManager().get(SpriteSheetId::SpaceShip).instanceSprite(0);

    helpers::centerOrigin(renderable, renderable.getLocalBounds());
    renderable.setPosition(400.0f, 300.0f);

    mRegistry.assign<Player>(player);
    mRegistry.assign<Health>(player, 3);
    mRegistry.assign<Fuel>(player, 20000.0f);
    mRegistry.assign<Velocity>(player);
    mRegistry.assign<RechargeTime>(player, RechargeTime(1.0f));
    mRegistry.assign<Renderable>(player, std::move(renderable));
}

void SolarSystem::onNotify(const PlanetExited &planetExited) noexcept {
    mRegistry.group<Planet, SceneSwitcher>().each([&](const auto &planetTag, const auto &sceneSwitcher) {
        (void) planetTag;

        if (planetExited.planetSceneId == sceneSwitcher.getSceneId()) {
            const auto players = mRegistry.view<Player>();
            mRegistry.destroy(players.begin(), players.end());
            mRegistry.create<Player, Health, Fuel, Velocity, RechargeTime, Renderable>(planetExited.playerId, planetExited.sourceRegistry);
        }
    });
}

void SolarSystem::onNotify(const PlanetDestroyed &planetDestroyed) noexcept {
    mRegistry.group<Planet, SceneSwitcher>().each([&](const auto planetId, const auto &planetTag, const auto &sceneSwitcher) {
        (void) planetTag;

        if (planetDestroyed.planetSceneId == sceneSwitcher.getSceneId()) {
            mRegistry.destroy(planetId);
        }
    });
}

SceneId SolarSystem::update(const sf::RenderWindow &window, Assets &assets, const sf::Time elapsed) noexcept {
    mNextSceneId = getSceneId();

    inputSystem(window, assets.getSpriteSheetsManager(), elapsed);
    motionSystem(elapsed);
    collisionSystem(window);
    livenessSystem();
    reportSystem(window);

    return mNextSceneId;
}

void SolarSystem::render(sf::RenderTarget &window) noexcept {
    window.draw(mReport);

    mRegistry.view<const Renderable>().each([&](const auto &renderable) {
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

void SolarSystem::inputSystem(const sf::RenderWindow &window, const SpriteSheetsManager &spriteSheetsManager, const sf::Time elapsed) noexcept {
    mRegistry.view<Player, Renderable, Velocity, Fuel, RechargeTime>().each([&](const auto &playerTag, auto &renderable, auto &velocity, auto &fuel, auto &rechargeTime) {
        (void) playerTag;
        auto speed = SPEED;
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
        rechargeTime.elapse(elapsed);

        if (rechargeTime.canShoot() and sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            rechargeTime.reset();
            const auto bulletEntity = mRegistry.create();
            auto bulletRenderable = spriteSheetsManager.get(SpriteSheetId::Bullet).instanceSprite(0);

            helpers::centerOrigin(bulletRenderable, bulletRenderable.getLocalBounds());
            bulletRenderable.setPosition(renderable.getPosition() + helpers::makeVector2(renderable.getRotation(), 22.8f));

            mRegistry.assign<Bullet>(bulletEntity);
            mRegistry.assign<Velocity>(bulletEntity, helpers::makeVector2(renderable.getRotation(), 800.0f));
            mRegistry.assign<Renderable>(bulletEntity, std::move(bulletRenderable)); // this must be the last line in order to avoid dangling pointers
        }
    });
}

void SolarSystem::motionSystem(const sf::Time elapsed) noexcept {
    mRegistry.group<Renderable, Velocity>().each([&](auto &renderable, const auto &velocity) {
        renderable.move(velocity.value * elapsed.asSeconds());
    });
}

void SolarSystem::collisionSystem(const sf::RenderWindow &window) noexcept {
    const auto viewport = sf::FloatRect(window.getViewport(window.getView()));

    mRegistry.view<Player, Renderable>().each([&](const auto &playerId, const auto &playerTag, auto &playerRenderable) {
        (void) playerTag;

        if (not viewport.intersects(playerRenderable.getHitBox())) {
            mRegistry.get<Health>(playerId).value -= 1;
            playerRenderable.setPosition({viewport.width / 2, viewport.height / 2});
        }

        mRegistry.view<Planet, SceneSwitcher, Renderable>().each([&](const auto &planetTag, const auto &sceneSwitcher, const auto &planetRenderable) {
            (void) planetTag;

            if (playerRenderable.getHitBox().intersects(planetRenderable.getHitBox())) {
                playerRenderable.setPosition({400.0f, 300.0f});
                pubsub::notify<PlanetEntered>({sceneSwitcher.getSceneId(), playerId, mRegistry});
                mNextSceneId = sceneSwitcher.getSceneId();
            }
        });
    });

    mRegistry.view<Bullet, Renderable>().each([&](const auto &bulletId, const auto &bulletTag, const auto &bulletRenderable) {
        (void) bulletTag;

        if (not viewport.intersects(bulletRenderable.getHitBox())) {
            mRegistry.destroy(bulletId);
        } else {
            mRegistry.view<Player, Renderable>().each([&](const auto &playerId, const auto &playerTag, auto &playerRenderable) {
                (void) playerTag;

                if (bulletRenderable.getHitBox().intersects(playerRenderable.getHitBox())) {
                    mRegistry.destroy(bulletId);
                    mRegistry.get<Health>(playerId).value -= 1;
                }
            });
        }
    });
}

void SolarSystem::livenessSystem() noexcept {
    auto livingPlanets = false;

    for (const auto &e : mRegistry.view<Planet>()) {
        (void) e;
        livingPlanets = true;
        break;
    }

    if (not livingPlanets) {
        mNextSceneId = mYouWonSceneId;
        return;
    }

    mRegistry.view<Player, Health, Fuel>().each([&](const auto &playerTag, const auto &health, const auto &fuel) {
        (void) playerTag;

        if (health.isDead() or fuel.isOver()) {
            // TODO maybe remove player entity
            mNextSceneId = mGameOverSceneId;
        }
    });
}

void SolarSystem::reportSystem(const sf::RenderWindow &window) noexcept {
    mRegistry.view<Player, Health, Fuel>().each([&](const auto &playerTag, const auto &health, const auto &fuel) {
        (void) playerTag;

        std::snprintf(mBuffer, std::size(mBuffer), "health: %d    fuel: %3.0f", health.value, fuel.value);

        mReport.setString(mBuffer);
        helpers::centerOrigin(mReport, mReport.getLocalBounds());

        mReport.setPosition(window.getSize().x / 2.0f, 18.0f);
    });
}

void SolarSystem::addPlanet(const SceneId sceneId) noexcept {
    // TODO: randomly generated position and color

    auto planet = mRegistry.create();
    auto renderable = sf::CircleShape(32, 256); // TODO: make planets sprite sheet

    helpers::centerOrigin(renderable, renderable.getLocalBounds());
    renderable.setPosition(200.0f, 150.0f);

    mRegistry.assign<Planet>(planet);
    mRegistry.assign<SceneSwitcher>(planet, sceneId);
    mRegistry.assign<Renderable>(planet, std::move(renderable));
}
