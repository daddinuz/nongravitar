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
#include <constants.hpp>
#include <components.hpp>
#include <scene/PlanetAssault.hpp>

using namespace gravitar;
using namespace gravitar::tags;
using namespace gravitar::scene;
using namespace gravitar::assets;
using namespace gravitar::messages;
using namespace gravitar::constants;
using namespace gravitar::components;

using RandomDevice = helpers::RandomDevice;
using IntDistribution = helpers::IntDistribution;
using FloatDistribution = helpers::FloatDistribution;

PlanetAssault::PlanetAssault(const SceneId solarSystemSceneId, const SceneId gameOverSceneId) :
        mBuffer{},
        mRandomEngine{RandomDevice()()},
        mGameOverSceneId{gameOverSceneId},
        mSolarSystemSceneId{solarSystemSceneId} {}

PlanetAssault &PlanetAssault::initialize(const sf::RenderWindow &window, Assets &assets, sf::Color terrainColor) noexcept {
    initializeGroups();
    initializeReport(assets);
    initializeTerrain(window, assets, terrainColor);
    initializePubSub();
    return *this;
}

SceneId PlanetAssault::update(const sf::RenderWindow &window, Assets &assets, const sf::Time elapsed) noexcept {
    mNextSceneId = getSceneId();

    if (auto &audioManager = assets.getAudioManager(); SoundTrackId::ComputerAdventures != audioManager.getPlaying()) {
        audioManager.play(SoundTrackId::ComputerAdventures);
    }

    inputSystem(assets, elapsed);
    motionSystem(elapsed);
    collisionSystem(window, assets);
    reloadSystem(elapsed);
    AISystem(assets);
    livenessSystem();
    reportSystem(window);

    return mNextSceneId;
}

void PlanetAssault::render(sf::RenderTarget &window) noexcept {
    window.draw(mReport);

    mRegistry.group<>(entt::get < Renderable > , entt::exclude < Hidden > ).each([&](const auto id, const auto &renderable) {
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

void PlanetAssault::operator()(const PlanetEntered &message) noexcept {
    if (message.sceneId == getSceneId()) {
        const auto players = mRegistry.view<Player>();
        const auto tractors = mRegistry.view<Tractor>();

        mRegistry.destroy(players.begin(), players.end());
        mRegistry.destroy(tractors.begin(), tractors.end());

        for (const auto sourcePlayerId : message.registry.view<Player>()) {
            auto tractorId = mRegistry.create();
            auto tractorRenderable = sf::CircleShape(TRACTOR_RADIUS, 256);

            helpers::centerOrigin(tractorRenderable, tractorRenderable.getLocalBounds());
            tractorRenderable.setFillColor(sf::Color::Transparent);
            tractorRenderable.setOutlineThickness(1.0f);
            tractorRenderable.setOutlineColor(sf::Color(100, 150, 250, 80));
            mRegistry.assign<Hidden>(tractorId);
            mRegistry.assign<Tractor>(tractorId);
            mRegistry.assign<HitRadius>(tractorId, TRACTOR_RADIUS);
            mRegistry.assign<Renderable>(tractorId, std::move(tractorRenderable));

            const auto playerId = mRegistry.create(sourcePlayerId, message.registry);
            mRegistry.assign<EntityRef<Tractor>>(playerId, tractorId);
            mRegistry.assign<EntityRef<Player>>(tractorId, playerId);

            auto &playerRenderable = mRegistry.get<Renderable>(playerId);
            playerRenderable->setRotation(90.0f);
            playerRenderable->setPosition(message.window.getSize().x / 2.0f, 128.0f);
        }
    }
}

void PlanetAssault::initializePubSub() const noexcept {
    pubsub::subscribe<messages::PlanetEntered>(*this);
}

void PlanetAssault::initializeGroups() noexcept {
    mRegistry.group<HitRadius, Renderable>();
    mRegistry.group<Velocity>(entt::get < Renderable > );

    mRegistry.group<AI1>(entt::get < ReloadTime, HitRadius, Renderable > );
    mRegistry.group<AI2>(entt::get < ReloadTime, HitRadius, Renderable > );

    mRegistry.group<Bunker>(entt::get < HitRadius, Renderable > );
    mRegistry.group<Bullet>(entt::get < HitRadius, Renderable > );
    mRegistry.group<Player>(entt::get < HitRadius, Renderable > );
    mRegistry.group<Terrain>(entt::get < HitRadius, Renderable > );

    mRegistry.group<Tractor>(entt::get < EntityRef<Player>, HitRadius, Renderable > , entt::exclude < Hidden > );
    mRegistry.group<Supply<Fuel>>(entt::get < HitRadius, Renderable > );
    mRegistry.group<Supply<Health>>(entt::get < HitRadius, Renderable > );

    mRegistry.group<>(entt::get < Renderable > , entt::exclude < Hidden > );
}

void PlanetAssault::initializeReport(Assets &assets) noexcept {
    mReport.setCharacterSize(18);
    mReport.setFillColor(sf::Color(105, 235, 245, 255));
    mReport.setFont(assets.getFontsManager().get(FontId::Mechanical));
}

void PlanetAssault::initializeTerrain(const sf::RenderWindow &window, Assets &assets, sf::Color terrainColor) noexcept {
    const auto halfWindowHeight = window.getSize().y / 2.0f;
    const auto viewport = sf::FloatRect(window.getViewport(window.getView()));

    const auto _terrainFrame = assets.getSpriteSheetsManager().get(SpriteSheetId::Terrain).getBuffer().at(0);
    const auto terrainHitDiameter = std::max(_terrainFrame.width, _terrainFrame.height);
    const auto terrainHitRadius = terrainHitDiameter / 2.0f;
    auto rotationDistribution = FloatDistribution(-38.0f, 38.0f);
    auto terrainPosition = sf::Vector2f(
            0.0f,
            FloatDistribution(halfWindowHeight * 1.5f + terrainHitDiameter, halfWindowHeight * 2.0f - terrainHitDiameter)(mRandomEngine)
    );

    terrainColor.a = 255;

    do {
        auto terrainId = mRegistry.create();
        auto terrainRenderable = assets.getSpriteSheetsManager().get(SpriteSheetId::Terrain).instanceSprite(0);
        const auto terrainBounds = terrainRenderable.getLocalBounds();
        const auto terrainRotation = rotationDistribution(mRandomEngine);
        const auto terrainOffset = helpers::makeVector2(terrainRotation, terrainHitRadius);

        helpers::centerOrigin(terrainRenderable, terrainBounds);

        terrainPosition += terrainOffset;
        terrainRenderable.setColor(terrainColor);
        terrainRenderable.setPosition(terrainPosition);
        terrainRenderable.setRotation(terrainRotation);
        terrainPosition += terrainOffset;

        mRegistry.assign<Terrain>(terrainId);
        mRegistry.assign<HitRadius>(terrainId, terrainHitRadius);
        mRegistry.assign<Renderable>(terrainId, std::move(terrainRenderable));
    } while (viewport.contains(terrainPosition));

    auto AI1ReloadDistribution = FloatDistribution(1.64f, 2.28f);
    auto AI2ReloadDistribution = FloatDistribution(1.96f, 2.28f);
    auto fuelSupplyDistribution = FloatDistribution(2000.0f, 5000.0f);
    auto healthSupplyDistribution = IntDistribution(1, 2);
    auto entityDistribution = IntDistribution(1, 18);

    mRegistry.view<Terrain, Renderable>().each([&](const auto terrainTag, const auto &terrainRenderable) {
        (void) terrainTag;

        switch (entityDistribution(mRandomEngine)) {
            case 2:
            case 16: {
                auto bunkerId = mRegistry.create();
                auto bunkerRenderable = assets.getSpriteSheetsManager().get(SpriteSheetId::Bunker).instanceSprite(0);
                const auto bunkerBounds = bunkerRenderable.getLocalBounds();
                const auto bunkerHitRadius = std::max(bunkerBounds.width, bunkerBounds.height) / 2.0f;

                helpers::centerOrigin(bunkerRenderable, bunkerBounds);

                bunkerRenderable.setRotation(terrainRenderable->getRotation() + 180.0f);
                bunkerRenderable.setPosition(terrainRenderable->getPosition());
                bunkerRenderable.move(helpers::makeVector2(terrainRenderable->getRotation() + 270.0f, bunkerHitRadius));

                mRegistry.assign<AI1>(bunkerId);
                mRegistry.assign<Bunker>(bunkerId);
                mRegistry.assign<Health>(bunkerId, 1);
                mRegistry.assign<ReloadTime>(bunkerId, AI1ReloadDistribution(mRandomEngine));
                mRegistry.assign<HitRadius>(bunkerId, bunkerHitRadius);
                mRegistry.assign<Renderable>(bunkerId, std::move(bunkerRenderable));
            }
                break;

            case 8: {
                auto bunkerId = mRegistry.create();
                auto bunkerRenderable = assets.getSpriteSheetsManager().get(SpriteSheetId::Bunker).instanceSprite(1);
                const auto bunkerBounds = bunkerRenderable.getLocalBounds();
                const auto bunkerHitRadius = std::max(bunkerBounds.width, bunkerBounds.height) / 2.0f;

                helpers::centerOrigin(bunkerRenderable, bunkerBounds);

                bunkerRenderable.setRotation(terrainRenderable->getRotation() + 180.0f);
                bunkerRenderable.setPosition(terrainRenderable->getPosition());
                bunkerRenderable.move(helpers::makeVector2(terrainRenderable->getRotation() + 270.0f, bunkerHitRadius));

                mRegistry.assign<AI2>(bunkerId);
                mRegistry.assign<Bunker>(bunkerId);
                mRegistry.assign<Health>(bunkerId, 2);
                mRegistry.assign<ReloadTime>(bunkerId, AI2ReloadDistribution(mRandomEngine));
                mRegistry.assign<HitRadius>(bunkerId, bunkerHitRadius);
                mRegistry.assign<Renderable>(bunkerId, std::move(bunkerRenderable));
            }
                break;

            case 4: {
                auto supplyId = mRegistry.create();
                auto supplyRenderable = sf::RectangleShape({24.0f, 24.0f});
                const auto supplyBounds = supplyRenderable.getLocalBounds();
                const auto supplyHitRadius = std::max(supplyBounds.width, supplyBounds.height) / 2.0f;

                supplyRenderable.setOutlineThickness(1.0f);
                supplyRenderable.setOutlineColor(sf::Color::Yellow);
                supplyRenderable.setFillColor(sf::Color::Transparent);

                helpers::centerOrigin(supplyRenderable, supplyBounds);

                supplyRenderable.setRotation(terrainRenderable->getRotation() + 180.0f);
                supplyRenderable.setPosition(terrainRenderable->getPosition());
                supplyRenderable.move(helpers::makeVector2(terrainRenderable->getRotation() + 270.0f, supplyHitRadius));

                mRegistry.assign<Supply<Fuel>>(supplyId, fuelSupplyDistribution(mRandomEngine));
                mRegistry.assign<HitRadius>(supplyId, supplyHitRadius);
                mRegistry.assign<Renderable>(supplyId, std::move(supplyRenderable));
            }
                break;

            case 12: {
                auto supplyId = mRegistry.create();
                auto supplyRenderable = sf::RectangleShape({24.0f, 24.0f});
                const auto supplyBounds = supplyRenderable.getLocalBounds();
                const auto supplyHitRadius = std::max(supplyBounds.width, supplyBounds.height) / 2.0f;

                supplyRenderable.setOutlineThickness(1.0f);
                supplyRenderable.setOutlineColor(sf::Color::Blue);
                supplyRenderable.setFillColor(sf::Color::Transparent);

                helpers::centerOrigin(supplyRenderable, supplyBounds);

                supplyRenderable.setRotation(terrainRenderable->getRotation() + 180.0f);
                supplyRenderable.setPosition(terrainRenderable->getPosition());
                supplyRenderable.move(helpers::makeVector2(terrainRenderable->getRotation() + 270.0f, supplyHitRadius));

                mRegistry.assign<Supply<Health>>(supplyId, healthSupplyDistribution(mRandomEngine));
                mRegistry.assign<HitRadius>(supplyId, supplyHitRadius);
                mRegistry.assign<Renderable>(supplyId, std::move(supplyRenderable));
            }
                break;

            default:
                break;
        }
    });
}

void PlanetAssault::addBullet(Assets &assets, const sf::Vector2f &position, const float rotation) noexcept {
    auto bulletRenderable = assets.getSpriteSheetsManager().get(SpriteSheetId::Bullet).instanceSprite(0);
    const auto bulletBounds = bulletRenderable.getLocalBounds();
    const auto bulletId = mRegistry.create();
    static const auto bulletHitRadius = std::max(bulletBounds.width, bulletBounds.height) / 2.0f;

    helpers::centerOrigin(bulletRenderable, bulletBounds);
    bulletRenderable.setRotation(rotation);
    bulletRenderable.setPosition(position);

    mRegistry.assign<Bullet>(bulletId);
    mRegistry.assign<Velocity>(bulletId, helpers::makeVector2(rotation, BULLET_SPEED));
    mRegistry.assign<HitRadius>(bulletId, bulletHitRadius);
    mRegistry.assign<Renderable>(bulletId, std::move(bulletRenderable));

    assets.getAudioManager().play(SoundId::Shot);
}

void PlanetAssault::inputSystem(Assets &assets, const sf::Time elapsed) noexcept {
    using Key = sf::Keyboard::Key;
    const auto isKeyPressed = &sf::Keyboard::isKeyPressed;

    mRegistry
            .view<Player, Fuel, Velocity, ReloadTime, HitRadius, Renderable>()
            .each([&](const auto playerId, const auto playerTag, auto &playerFuel, auto &playerVelocity,
                      auto &playerReloadTime, const auto &playerHitRadius, auto &playerRenderable) {
                (void) playerTag;
                const auto tractorId = *mRegistry.get<EntityRef<Tractor>>(playerId);
                auto playerSpeed = PLAYER_SPEED;

                if (isKeyPressed(Key::W)) {
                    playerSpeed *= 1.32f;
                } else if (isKeyPressed(Key::S)) {
                    playerSpeed *= 0.88f;
                }

                if (isKeyPressed(Key::A)) {
                    playerRenderable->rotate(-PLAYER_ROTATION_SPEED * elapsed.asSeconds());
                }

                if (isKeyPressed(Key::D)) {
                    playerRenderable->rotate(PLAYER_ROTATION_SPEED * elapsed.asSeconds());
                }

                playerVelocity.value = helpers::makeVector2(playerRenderable->getRotation(), playerSpeed);
                playerFuel.value -= playerSpeed * elapsed.asSeconds();

                if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                    mRegistry.get<Renderable>(tractorId)->setPosition(playerRenderable->getPosition());
                    mRegistry.reset<Hidden>(tractorId);
                } else {
                    mRegistry.assign_or_replace<Hidden>(tractorId);
                }

                if (playerReloadTime.canShoot() and sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    const auto bulletRotation = playerRenderable->getRotation();
                    const auto bulletPosition = playerRenderable->getPosition() + helpers::makeVector2(bulletRotation, *playerHitRadius + 1.0f);
                    playerReloadTime.reset();
                    addBullet(assets, bulletPosition, bulletRotation); // NOTE for a future me: be aware that this invalidates some component references !!!
                }
            });
}

void PlanetAssault::motionSystem(const sf::Time elapsed) noexcept {
    mRegistry.group<Velocity>(entt::get < Renderable > ).each([&](const auto &velocity, auto &renderable) {
        renderable->move(velocity.value * elapsed.asSeconds());
    });
}

void PlanetAssault::collisionSystem(const sf::RenderWindow &window, Assets &assets) noexcept {
    const auto viewport = sf::FloatRect(window.getViewport(window.getView()));
    const auto players = mRegistry.group<Player>(entt::get < HitRadius, Renderable > );

    for (const auto playerId : players) {
        auto &playerRenderable = players.get<Renderable>(playerId);

        if (not viewport.contains(playerRenderable->getPosition())) {
            const auto bullets = mRegistry.view<Bullet>();
            mNextSceneId = mSolarSystemSceneId;
            mRegistry.destroy(bullets.begin(), bullets.end());
            pubsub::publish<SolarSystemEntered>(window, mRegistry, getSceneId());
            playerRenderable->setPosition(sf::Vector2f(window.getSize()) / 2.0f);
            return;
        }
    }

    auto bulletsToDestroy = std::set<entt::entity>();
    mRegistry.group<Bullet>(entt::get < HitRadius, Renderable > ).each([&](const auto bulletId, const auto bulletTag, const auto &bulletHitRadius, const auto &bulletRenderable) {
        (void) bulletTag;

        if (viewport.contains(bulletRenderable->getPosition())) {
            mRegistry.group<HitRadius, Renderable>().each([&](const auto entityId, const auto &entityHitRadius, const auto &entityRenderable) {
                if (entityId != bulletId and helpers::magnitude(entityRenderable->getPosition(), bulletRenderable->getPosition()) <= *entityHitRadius + *bulletHitRadius) {
                    if (not mRegistry.has<Tractor>(entityId)) {
                        bulletsToDestroy.insert(bulletId);
                    }

                    if (mRegistry.has<Player>(entityId) or mRegistry.has<Bunker>(entityId)) {
                        assets.getAudioManager().play(SoundId::Hit);
                        mRegistry.get<Health>(entityId).value -= 1;
                    }
                }
            });
        } else {
            bulletsToDestroy.insert(bulletId);
        }
    });
    mRegistry.destroy(bulletsToDestroy.begin(), bulletsToDestroy.end());

    mRegistry.group<Terrain>(entt::get < HitRadius, Renderable > ).each([&](const auto terrainTag, const auto &terrainHitRadius, const auto &terrainRenderable) {
        (void) terrainTag;

        mRegistry
                .group<Player>(entt::get < HitRadius, Renderable > )
                .each([&](const auto playerId, const auto playerTag, const auto &playerHitRadius, auto &playerRenderable) {
                    (void) playerTag;

                    if (helpers::magnitude(terrainRenderable->getPosition(), playerRenderable->getPosition()) <= *terrainHitRadius + *playerHitRadius) {
                        playerRenderable->setPosition(sf::Vector2f(window.getSize()) / 2.0f);
                        mRegistry.get<Health>(playerId).value -= 1;
                    }
                });
    });

    mRegistry.group<Bunker>(entt::get < HitRadius, Renderable > ).each([&](const auto bunkerTag, const auto &bunkerHitRadius, const auto &bunkerRenderable) {
        (void) bunkerTag;

        mRegistry
                .group<Player>(entt::get < HitRadius, Renderable > )
                .each([&](const auto playerId, const auto playerTag, const auto &playerHitRadius, auto &playerRenderable) {
                    (void) playerTag;

                    if (helpers::magnitude(bunkerRenderable->getPosition(), playerRenderable->getPosition()) <= *bunkerHitRadius + *playerHitRadius) {
                        playerRenderable->setPosition(sf::Vector2f(window.getSize()) / 2.0f);
                        mRegistry.get<Health>(playerId).value -= 1;
                    }
                });
    });

    mRegistry
            .group<Tractor>(entt::get < EntityRef<Player>, HitRadius, Renderable > , entt::exclude < Hidden > )
            .each([&](const auto tractorTag, const auto &playerRef, const auto &tractorHitRadius, const auto &tractorRenderable) {
                (void) tractorTag;
                const auto playerId = *playerRef;

                mRegistry
                        .group<Supply<Fuel>>(entt::get < HitRadius, Renderable > )
                        .each([&](const auto supplyId, const auto &supply, const auto &supplyHitRadius, const auto &supplyRenderable) {
                            if (helpers::magnitude(tractorRenderable->getPosition(), supplyRenderable->getPosition()) <= *tractorHitRadius + *supplyHitRadius) {
                                assets.getAudioManager().play(SoundId::Tractor);
                                mRegistry.get<Fuel>(playerId).value += supply->value;
                                mRegistry.destroy(supplyId);
                            }
                        });

                mRegistry
                        .group<Supply<Health>>(entt::get < HitRadius, Renderable > )
                        .each([&](const auto supplyId, const auto &supply, const auto &supplyHitRadius, const auto &supplyRenderable) {
                            if (helpers::magnitude(tractorRenderable->getPosition(), supplyRenderable->getPosition()) <= *tractorHitRadius + *supplyHitRadius) {
                                assets.getAudioManager().play(SoundId::Tractor);
                                mRegistry.get<Health>(playerId).value += supply->value;
                                mRegistry.destroy(supplyId);
                            }
                        });
            });
}

void PlanetAssault::reloadSystem(sf::Time elapsed) noexcept {
    mRegistry.view<ReloadTime>().each([&](auto &reloadTime) {
        reloadTime.elapse(elapsed);
    });
}

void PlanetAssault::AISystem(Assets &assets) noexcept {
    auto AI1Precision = FloatDistribution(-32.0f, 32.0f);
    auto AI2Precision = FloatDistribution(-8.0f, 8.0f);

    mRegistry.view<Player, Renderable>().each([&](const auto playerTag, const auto playerRenderable) {
        (void) playerTag;

        mRegistry.group<AI1>(entt::get < ReloadTime, HitRadius, Renderable > ).each([&](const auto AITag, auto &AIReloadTime, const auto &AIHitRadius, const auto &AIRenderable) {
            (void) AITag;

            if (AIReloadTime.canShoot()) {
                const auto bulletRotation = helpers::rotation(AIRenderable->getPosition(), playerRenderable->getPosition()) +
                                            AI1Precision(mRandomEngine);
                const auto bulletPosition = AIRenderable->getPosition() + helpers::makeVector2(bulletRotation, *AIHitRadius + 1.0f);
                AIReloadTime.reset();
                addBullet(assets, bulletPosition, bulletRotation);
            }
        });

        mRegistry.group<AI2>(entt::get < ReloadTime, HitRadius, Renderable > ).each(
                [&](const auto AITag, auto &AIReloadTime, const auto &AIHitRadius, const auto &AIRenderable) {
                    (void) AITag;

                    if (AIReloadTime.canShoot()) {
                        const auto bulletRotation = helpers::rotation(AIRenderable->getPosition(), playerRenderable->getPosition()) +
                                                    AI2Precision(mRandomEngine);
                        const auto bulletPosition = AIRenderable->getPosition() + helpers::makeVector2(bulletRotation, *AIHitRadius + 1.0f);
                        AIReloadTime.reset();
                        addBullet(assets, bulletPosition, bulletRotation);
                    }
                });
    });
}

void PlanetAssault::livenessSystem() noexcept {
    mRegistry.view<Player, Health, Fuel>().each([&](const auto id, const auto tag, const auto &health, const auto &fuel) {
        (void) tag;

        if (health.isDead() or fuel.isOver()) {
            mRegistry.destroy(id);
            mNextSceneId = mGameOverSceneId;
        }
    });

    mRegistry.view<Bunker, Health>().each([&](const auto id, const auto tag, const auto &health) {
        (void) tag;

        if (health.isDead()) {
            mRegistry.destroy(id);
        }
    });
}

void PlanetAssault::reportSystem(const sf::RenderWindow &window) noexcept {
    mRegistry.view<Player, Health, Fuel>().each([&](const auto tag, const auto &health, const auto &fuel) {
        (void) tag;

        std::snprintf(mBuffer, std::size(mBuffer), "health: %d fuel: %.0f", health.value, fuel.value);
        helpers::centerOrigin(mReport, mReport.getLocalBounds());

        mReport.setString(mBuffer);
        mReport.setPosition(window.getSize().x / 2.0f, 18.0f);
    });
}
