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
#include <pubsub.hpp>
#include <helpers.hpp>
#include <messages.hpp>
#include <components.hpp>
#include <scene/PlanetAssault.hpp>

using namespace gravitar;
using namespace gravitar::tags;
using namespace gravitar::scene;
using namespace gravitar::assets;
using namespace gravitar::messages;
using namespace gravitar::components;

constexpr float SPEED = 180.0f;
constexpr float ROTATION_SPEED = 180.0f;

PlanetAssault::PlanetAssault(const SceneId gameOverSceneId, Assets &assets) :
        mBuffer{},
        mGameOverSceneId{gameOverSceneId} {
    mReport.setCharacterSize(18);
    mReport.setFillColor(sf::Color(105, 235, 245, 255));
    mReport.setFont(assets.getFontsManager().get(FontId::Mechanical));

    mRegistry.group<Renderable, Velocity>();
    mRegistry.group<Player>(entt::get < Renderable > );
    mRegistry.group<Bunker>(entt::get < Renderable > );
    mRegistry.group<Bullet>(entt::get < Renderable > );

    mRegistry.group<>(entt::get < Player, Health, Fuel > );
    mRegistry.group<>(entt::get < Bunker, Health > );
}

SceneId PlanetAssault::update(const sf::RenderWindow &window, Assets &assets, const sf::Time elapsed) noexcept {
    mNextSceneId = getSceneId();

    inputSystem(window, assets.getSpriteSheetsManager(), elapsed);
    motionSystem(elapsed);
    collisionSystem(window);
    livenessSystem();
    reportSystem(window);

    return mNextSceneId;
}

void PlanetAssault::render(sf::RenderTarget &window) noexcept {
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

void PlanetAssault::addBunker(const sf::RenderWindow &window, Assets &assets) noexcept {
    (void) window;
    (void) assets;

    // TODO randomly generate bunkers
    auto bunkerId = mRegistry.create();
    auto bunkerRenderable = sf::CircleShape(32, 8);

    helpers::centerOrigin(bunkerRenderable, bunkerRenderable.getLocalBounds());
    bunkerRenderable.setPosition(400.0f, 300.0f);
    bunkerRenderable.setFillColor(sf::Color::Red);

    mRegistry.assign<Bunker>(bunkerId);
    mRegistry.assign<Health>(bunkerId, 1);
    mRegistry.assign<Renderable>(bunkerId, std::move(bunkerRenderable));
}

void PlanetAssault::addTerrain(const sf::RenderWindow &window, Assets &assets) noexcept {
    (void) window;
    (void) assets;
    // TODO terrain generation
}

void PlanetAssault::setParentSceneId(SceneId parentSceneId) noexcept {
    mParentSceneId = parentSceneId;
}

SceneId PlanetAssault::getParentSceneId() const noexcept {
    return mParentSceneId;
}

void PlanetAssault::operator()(const PlanetEntered &planetEntered) noexcept {
    if (planetEntered.planetSceneId == getSceneId()) {
        const auto players = mRegistry.view<Player>();
        mRegistry.destroy(players.begin(), players.end());
        mRegistry.create<Player, Fuel, Health, Velocity, RechargeTime, Renderable>(planetEntered.playerId, planetEntered.sourceRegistry);
    }
}

void PlanetAssault::inputSystem(const sf::RenderWindow &window, const SpriteSheetsManager &spriteSheetsManager, const sf::Time elapsed) noexcept {
    mRegistry.group<Player>(entt::get < Renderable > ).each([&](const auto playerId, const auto playerTag, auto &renderable) {
        (void) playerTag;

        auto speed = SPEED;
        const auto &[fuel, velocity, rechargeTime] = mRegistry.get<Fuel, Velocity, RechargeTime>(playerId);
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
            const auto bulletId = mRegistry.create();
            auto bulletRenderable = spriteSheetsManager.get(SpriteSheetId::Bullet).instanceSprite(0);

            helpers::centerOrigin(bulletRenderable, bulletRenderable.getLocalBounds());
            bulletRenderable.setPosition(renderable.getPosition() + helpers::makeVector2(renderable.getRotation(), 22.8f));

            mRegistry.assign<Bullet>(bulletId);
            mRegistry.assign<Velocity>(bulletId, helpers::makeVector2(renderable.getRotation(), 800.0f));
            mRegistry.assign<Renderable>(bulletId, std::move(bulletRenderable)); // this must be the last line in order to avoid dangling pointers
        }
    });
}

void PlanetAssault::motionSystem(const sf::Time elapsed) noexcept {
    mRegistry.group<Renderable, Velocity>().each([&](auto &renderable, const auto &velocity) {
        renderable.move(velocity.value * elapsed.asSeconds());
    });
}

void PlanetAssault::collisionSystem(const sf::RenderWindow &window) noexcept {
    const auto viewport = sf::FloatRect(window.getViewport(window.getView()));

    mRegistry.group<Player>(entt::get < Renderable > ).each([&](const auto playerId, const auto playerTag, auto &playerRenderable) {
        (void) playerTag;

        if (not playerRenderable.getHitBox().intersects(viewport)) { // player-window intersection
            mNextSceneId = getParentSceneId();
            playerRenderable.setPosition(sf::Vector2f(window.getSize()) / 2.0f);
            pubsub::publish<PlanetExited>(getSceneId(), playerId, mRegistry);
        } else { // player-bunker intersection
            mRegistry.group<Bunker>(entt::get < Renderable > ).each([&](const auto bunkerTag, const auto &bunkerRenderable) {
                (void) bunkerTag;

                if (playerRenderable.getHitBox().intersects(bunkerRenderable.getHitBox())) {
                    mRegistry.get<Health>(playerId).value -= 1;
                    playerRenderable.setPosition(sf::Vector2f(window.getSize()) / 2.0f);
                }
            });
        }
    });

    mRegistry.group<Bullet>(entt::get < Renderable > ).each([&](const auto bulletId, const auto bulletTag, const auto &bulletRenderable) {
        (void) bulletTag;

        if (not bulletRenderable.getHitBox().intersects(viewport)) { // bullet-window intersection
            mRegistry.destroy(bulletId);
        } else {
            auto bulletsToDestroy = std::vector<entt::entity>();

            // bullet-player intersection
            mRegistry.view<Player, Renderable>().each([&](const auto playerId, const auto playerTag, const auto &playerRenderable) {
                (void) playerTag;

                if (bulletRenderable.getHitBox().intersects(playerRenderable.getHitBox())) {
                    mRegistry.get<Health>(playerId).value -= 1;
                    bulletsToDestroy.push_back(bulletId);
                }
            });

            // bullet-bunker intersection
            mRegistry.view<Bunker, Renderable>().each([&](const auto bunkerId, const auto bunkerTag, const auto &bunkerRenderable) {
                (void) bunkerTag;

                if (bulletRenderable.getHitBox().intersects(bunkerRenderable.getHitBox())) {
                    mRegistry.get<Health>(bunkerId).value -= 1;
                    bulletsToDestroy.push_back(bulletId);
                }
            });

            // TODO handle bullet-terrain collision

            mRegistry.destroy(bulletsToDestroy.begin(), bulletsToDestroy.end());
        }
    });
}

void PlanetAssault::livenessSystem() noexcept {
    auto livingBunkers = false;

    for (const auto bunkerId : mRegistry.view<Bunker>()) {
        (void) bunkerId;
        livingBunkers = true;
        break;
    }

    if (not livingBunkers) {
        mNextSceneId = mParentSceneId;
        pubsub::publish<PlanetExited>(getSceneId(), *mRegistry.view<Player>().begin(), mRegistry);
        pubsub::publish<PlanetDestroyed>(getSceneId());
    }

    mRegistry.group<>(entt::get < Player, Health, Fuel > ).each([&](const auto playerId, const auto playerTag, const auto &health, const auto &fuel) {
        (void) playerTag;

        if (health.isDead() or fuel.isOver()) {
            mRegistry.destroy(playerId);
            mNextSceneId = mGameOverSceneId;
        }
    });

    mRegistry.group<>(entt::get < Bunker, Health > ).each([&](const auto playerId, const auto bunkerTag, const auto &health) {
        (void) bunkerTag;

        if (health.isDead()) {
            mRegistry.destroy(playerId);
        }
    });
}

void PlanetAssault::reportSystem(const sf::RenderWindow &window) noexcept {
    mRegistry.group<>(entt::get < Player, Health, Fuel > ).each([&](const auto playerTag, const auto &health, const auto &fuel) {
        (void) playerTag;

        std::snprintf(mBuffer, std::size(mBuffer), "health: %d    fuel: %3.0f", health.value, fuel.value);

        mReport.setString(mBuffer);
        helpers::centerOrigin(mReport, mReport.getLocalBounds());

        mReport.setPosition(window.getSize().x / 2.0f, 18.0f);
    });
}
