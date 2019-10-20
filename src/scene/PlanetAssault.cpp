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

using namespace nongravitar;
using namespace nongravitar::tags;
using namespace nongravitar::scene;
using namespace nongravitar::assets;
using namespace nongravitar::messages;
using namespace nongravitar::constants;
using namespace nongravitar::components;

using helpers::RandomDevice;
using helpers::IntDistribution;
using helpers::FloatDistribution;

constexpr auto TERRAIN_SEGMENTS_PER_UNIT = 4u;

void shoot(entt::registry &registry, Assets &assets, const sf::Vector2f &position, float rotation) noexcept;

PlanetAssault::PlanetAssault(const SceneId solarSystemSceneId, const SceneId leaderBoardSceneId) :
        mBuffer{},
        mRandomEngine{RandomDevice()()},
        mLeaderBoardSceneId{leaderBoardSceneId},
        mSolarSystemSceneId{solarSystemSceneId} {}

PlanetAssault &PlanetAssault::initialize(const sf::RenderWindow &window, Assets &assets, sf::Color terrainColor) noexcept {
    initializeGroups();
    initializeReport(assets);
    initializeTerrain(window, assets, terrainColor);
    initializePubSub();
    return *this;
}

SceneId PlanetAssault::update(const sf::RenderWindow &window, SceneManager &, Assets &assets, const sf::Time elapsed) noexcept {
    mNextSceneId = getSceneId();

    if (auto &audioManager = assets.getAudioManager(); SoundTrackId::ComputerAdventures != audioManager.getPlaying()) {
        audioManager.play(SoundTrackId::ComputerAdventures);
    }

    inputSystem(assets, elapsed);
    motionSystem(elapsed);
    collisionSystem(window, assets, elapsed);
    reloadSystem(elapsed);
    AISystem(assets);
    livenessSystem(assets);
    reportSystem(window);

    return mNextSceneId;
}

void PlanetAssault::render(sf::RenderTarget &window) const noexcept {
    window.draw(mReport);

    mRegistry.group<const Renderable>(entt::exclude < Hidden > ).each([&](const auto id, const auto &renderable) {
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
            const auto[windowWidth, windowHeight] = sf::Vector2f(message.window.getSize());
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
            playerRenderable->setPosition(windowWidth / 2.0f, windowHeight / 4.0f);
        }
    }
}

void PlanetAssault::initializePubSub() const noexcept {
    pubsub::subscribe<messages::PlanetEntered>(*this);
}

void PlanetAssault::initializeGroups() noexcept {
    // render
    mRegistry.group<const Renderable>(entt::exclude < Hidden > );

    // motionSystem
    mRegistry.group<Velocity>(entt::get < Renderable > );

    // collisionSystem
    mRegistry.group<Health>(entt::get < Renderable, HitRadius > );
    mRegistry.group<Damage>(entt::get < Renderable, HitRadius > );

    mRegistry.group<Player>(entt::get < Renderable, HitRadius > );
    mRegistry.group<Terrain>(entt::get < Renderable, HitRadius > );
    mRegistry.group<Tractor>(entt::get < Renderable, HitRadius, EntityRef<Player>> , entt::exclude < Hidden > );
    mRegistry.group<Bullet>(entt::get < Renderable, HitRadius, Velocity > );
    mRegistry.group<Supply<Energy>>(entt::get < Renderable, HitRadius > );
    mRegistry.group<Supply<Health>>(entt::get < Renderable, HitRadius > );

    // AISystem
    mRegistry.group<AI1>(entt::get < Renderable, HitRadius, ReloadTime > );
    mRegistry.group<AI2>(entt::get < Renderable, HitRadius, ReloadTime > );
}

void PlanetAssault::initializeReport(Assets &assets) noexcept {
    mReport.setCharacterSize(18);
    mReport.setFillColor(sf::Color(105, 235, 245, 255));
    mReport.setFont(assets.getFontsManager().get(FontId::Mechanical));
}

void PlanetAssault::initializeTerrain(const sf::RenderWindow &window, Assets &assets, const sf::Color terrainColor) noexcept {
    const auto halfWindowHeight = window.getSize().y / 2.0f;
    const auto viewport = sf::FloatRect(window.getViewport(window.getView()));

    const auto terrainFrame = assets.getSpriteSheetsManager().get(SpriteSheetId::Terrain).getRect({0, 0});
    const auto terrainHitDiameter = std::max(terrainFrame.width, terrainFrame.height);
    const auto terrainHitRadius = terrainHitDiameter / 2.0f;
    auto rotationDistribution = FloatDistribution(-32.0f, 32.0f);
    auto terrainPosition = sf::Vector2f(
            0.0f,
            FloatDistribution(halfWindowHeight * 1.5f + terrainHitDiameter, halfWindowHeight * 2.0f - terrainHitDiameter)(mRandomEngine)
    );

    do {
        const auto terrainRotation = rotationDistribution(mRandomEngine);

        for (auto i = 0u; i < TERRAIN_SEGMENTS_PER_UNIT; i++) {
            auto terrainId = mRegistry.create();
            auto terrainRenderable = assets.getSpriteSheetsManager().get(SpriteSheetId::Terrain).getSprite({0, 0});
            const auto terrainBounds = terrainRenderable.getLocalBounds();
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
        }
    } while (viewport.contains(terrainPosition));

    auto AI1ReloadDistribution = FloatDistribution(1.64f, 2.28f);
    auto AI2ReloadDistribution = FloatDistribution(1.96f, 2.28f);
    auto energySupplyDistribution = FloatDistribution(2000.0f, 4000.0f);
    auto entityDistribution = IntDistribution(1, 16);

    const auto terrain = mRegistry.view<Terrain, Renderable>();
    for (auto terrainCursor = terrain.begin(); terrainCursor != terrain.end(); std::advance(terrainCursor, TERRAIN_SEGMENTS_PER_UNIT)) {
        const auto &terrainRenderable = terrain.get<Renderable>(*terrainCursor);
        const auto position = terrainRenderable->getPosition() +
                              helpers::makeVector2(terrainRenderable->getRotation() + 180.0f, terrainHitRadius * (TERRAIN_SEGMENTS_PER_UNIT - 1u));

        switch (entityDistribution(mRandomEngine)) {
            case 2:
            case 16: {
                auto bunkerId = mRegistry.create();
                auto bunkerRenderable = assets.getSpriteSheetsManager().get(SpriteSheetId::Bunker).getSprite({0, 0});
                const auto bunkerBounds = bunkerRenderable.getLocalBounds();
                const auto bunkerHitRadius = std::max(bunkerBounds.width, bunkerBounds.height) / 2.0f;

                helpers::centerOrigin(bunkerRenderable, bunkerBounds);
                bunkerRenderable.setRotation(terrainRenderable->getRotation() + 180.0f);
                bunkerRenderable.setPosition(position + helpers::makeVector2(terrainRenderable->getRotation() + 270.0f, bunkerHitRadius));

                mRegistry.assign<AI1>(bunkerId);
                mRegistry.assign<Bunker>(bunkerId);
                mRegistry.assign<Damage>(bunkerId, 2);
                mRegistry.assign<Health>(bunkerId, 1);
                mRegistry.assign<ReloadTime>(bunkerId, AI1ReloadDistribution(mRandomEngine));
                mRegistry.assign<HitRadius>(bunkerId, bunkerHitRadius);
                mRegistry.assign<Renderable>(bunkerId, std::move(bunkerRenderable));
            }
                break;

            case 8: {
                auto bunkerId = mRegistry.create();
                auto bunkerRenderable = assets.getSpriteSheetsManager().get(SpriteSheetId::Bunker).getSprite({1, 0});
                const auto bunkerBounds = bunkerRenderable.getLocalBounds();
                const auto bunkerHitRadius = std::max(bunkerBounds.width, bunkerBounds.height) / 2.0f;

                helpers::centerOrigin(bunkerRenderable, bunkerBounds);
                bunkerRenderable.setRotation(terrainRenderable->getRotation() + 180.0f);
                bunkerRenderable.setPosition(position + helpers::makeVector2(terrainRenderable->getRotation() + 270.0f, bunkerHitRadius));

                mRegistry.assign<AI2>(bunkerId);
                mRegistry.assign<Bunker>(bunkerId);
                mRegistry.assign<Damage>(bunkerId, 3);
                mRegistry.assign<Health>(bunkerId, 2);
                mRegistry.assign<ReloadTime>(bunkerId, AI2ReloadDistribution(mRandomEngine));
                mRegistry.assign<HitRadius>(bunkerId, bunkerHitRadius);
                mRegistry.assign<Renderable>(bunkerId, std::move(bunkerRenderable));
            }
                break;

            case 4: {
                auto supplyId = mRegistry.create();
                auto supplyRenderable = assets.getSpriteSheetsManager().get(SpriteSheetId::Supply).getSprite({1, 0});
                const auto supplyBounds = supplyRenderable.getLocalBounds();
                const auto supplyHitRadius = std::max(supplyBounds.width, supplyBounds.height) / 2.0f;

                helpers::centerOrigin(supplyRenderable, supplyBounds);
                supplyRenderable.setRotation(terrainRenderable->getRotation() + 180.0f);
                supplyRenderable.setPosition(position + helpers::makeVector2(terrainRenderable->getRotation() + 270.0f, supplyHitRadius));

                mRegistry.assign<Damage>(supplyId, 1);
                mRegistry.assign<Health>(supplyId, 1);
                mRegistry.assign<HitRadius>(supplyId, supplyHitRadius);
                mRegistry.assign<Renderable>(supplyId, std::move(supplyRenderable));
                mRegistry.assign<Supply<Energy>>(supplyId, energySupplyDistribution(mRandomEngine));
            }
                break;

            case 6:
            case 12: {
                auto supplyId = mRegistry.create();
                auto supplyRenderable = assets.getSpriteSheetsManager().get(SpriteSheetId::Supply).getSprite({0, 0});
                const auto supplyBounds = supplyRenderable.getLocalBounds();
                const auto supplyHitRadius = std::max(supplyBounds.width, supplyBounds.height) / 2.0f;

                helpers::centerOrigin(supplyRenderable, supplyBounds);
                supplyRenderable.setRotation(terrainRenderable->getRotation() + 180.0f);
                supplyRenderable.setPosition(position + helpers::makeVector2(terrainRenderable->getRotation() + 270.0f, supplyHitRadius));

                mRegistry.assign<Damage>(supplyId, 1);
                mRegistry.assign<Health>(supplyId, 1);
                mRegistry.assign<HitRadius>(supplyId, supplyHitRadius);
                mRegistry.assign<Renderable>(supplyId, std::move(supplyRenderable));
                mRegistry.assign<Supply<Health>>(supplyId, 1);
            }
                break;

            default:
                break;
        }
    }

    mRegistry.view<Bunker, Renderable>().each([&](const auto bunkerId, const auto, const auto &bunkerRenderable) {
        if (not viewport.contains(bunkerRenderable->getPosition())) {
            mRegistry.destroy(bunkerId);
        }
    });

    mBonus += SCORE_PER_AI1 * std::distance(mRegistry.view<AI1>().begin(), mRegistry.view<AI1>().end());
    mBonus += SCORE_PER_AI2 * std::distance(mRegistry.view<AI2>().begin(), mRegistry.view<AI2>().end());
}

void PlanetAssault::inputSystem(Assets &assets, const sf::Time elapsed) noexcept {
    using Key = sf::Keyboard::Key;
    const auto isKeyPressed = &sf::Keyboard::isKeyPressed;

    mRegistry
            .view<Player, HitRadius, Renderable, Energy, Velocity, ReloadTime>()
            .each([&](const auto playerId, const auto, const auto &playerHitRadius, auto &playerRenderable,
                      auto &playerEnergy, auto &playerVelocity, auto &playerReloadTime) {
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
                playerEnergy.consume(playerSpeed * elapsed.asSeconds());

                if (isKeyPressed(Key::RShift)) {
                    mRegistry.get<Renderable>(tractorId)->setPosition(playerRenderable->getPosition());
                    mRegistry.reset<Hidden>(tractorId);
                } else {
                    mRegistry.assign_or_replace<Hidden>(tractorId);

                    if (playerReloadTime.canShoot() and isKeyPressed(Key::Space)) {
                        const auto bulletRotation = playerRenderable->getRotation();
                        const auto bulletPosition = playerRenderable->getPosition() + helpers::makeVector2(bulletRotation, 1.0f + *playerHitRadius);
                        playerReloadTime.reset();
                        // NOTE for a future me: be aware that this invalidates some component references !!!
                        shoot(mRegistry, assets, bulletPosition, bulletRotation);
                    }
                }
            });
}

void PlanetAssault::motionSystem(const sf::Time elapsed) noexcept {
    mRegistry.group<Velocity>(entt::get < Renderable > ).each([&](const auto &velocity, auto &renderable) {
        renderable->move(velocity.value * elapsed.asSeconds());
    });
}

void PlanetAssault::collisionSystem(const sf::RenderWindow &window, Assets &assets, const sf::Time elapsed) noexcept {
    const auto viewport = sf::FloatRect(window.getViewport(window.getView()));
    auto solarSystemExited = false;
    auto isTractorActive = false;

    // general entities collisions
    const auto g1 = mRegistry.group<Health>(entt::get < Renderable, HitRadius > );
    const auto g2 = mRegistry.group<Damage>(entt::get < Renderable, HitRadius > );
    for (auto e1 : g1) {
        for (auto e2 : g2) {
            if (e1 != e2) {
                const auto &[entityRenderable1, entityHitRadius1] = g1.get<Renderable, HitRadius>(e1);
                const auto &[entityRenderable2, entityHitRadius2] = g2.get<Renderable, HitRadius>(e2);

                if (helpers::magnitude(entityRenderable1->getPosition(), entityRenderable2->getPosition()) <= *entityHitRadius1 + *entityHitRadius2) {
                    assets.getAudioManager().play(SoundId::Hit);
                    g1.get<Health>(e1).harm(g2.get<Damage>(e2));
                }
            }
        }
    }

    // tractor hits supply
    mRegistry
            .group<Tractor>(entt::get < Renderable, HitRadius, EntityRef<Player>> , entt::exclude < Hidden > )
            .each([&](const auto, const auto &tractorRenderable, const auto &tractorHitRadius, const auto &playerRef) {
                const auto playerId = *playerRef;

                mRegistry
                        .group<Bullet>(entt::get < Renderable, HitRadius, Velocity > )
                        .each([&](const auto, auto &bulletRenderable, const auto &bulletHitRadius, auto &velocity) {
                            if (helpers::magnitude(tractorRenderable->getPosition(), bulletRenderable->getPosition()) <= *tractorHitRadius + *bulletHitRadius) {
                                const auto rotationDiff = helpers::shortestRotation(
                                        bulletRenderable->getRotation(),
                                        helpers::rotation(bulletRenderable->getPosition(), tractorRenderable->getPosition())
                                );

                                bulletRenderable->rotate(helpers::signum(rotationDiff) * 220.0f * elapsed.asSeconds());
                                velocity.value = helpers::makeVector2(bulletRenderable->getRotation(), BULLET_SPEED);
                            }
                        });

                mRegistry
                        .group<Supply<Energy>>(entt::get < Renderable, HitRadius > )
                        .each([&](const auto supplyId, const auto &supply, const auto &supplyRenderable, const auto &supplyHitRadius) {
                            if (helpers::magnitude(tractorRenderable->getPosition(), supplyRenderable->getPosition()) <= *tractorHitRadius + *supplyHitRadius) {
                                mRegistry.get<Health>(supplyId).kill();
                                mRegistry.get<Energy>(playerId).recharge(supply);
                                isTractorActive = true;
                            }
                        });

                mRegistry
                        .group<Supply<Health>>(entt::get < Renderable, HitRadius > )
                        .each([&](const auto supplyId, const auto &supply, const auto &supplyRenderable, const auto &supplyHitRadius) {
                            if (helpers::magnitude(tractorRenderable->getPosition(), supplyRenderable->getPosition()) <= *tractorHitRadius + *supplyHitRadius) {
                                mRegistry.get<Health>(supplyId).kill();
                                mRegistry.get<Health>(playerId).heal(supply);
                                isTractorActive = true;
                            }
                        });
            });

    // bullet exits screen / bullet hits terrain
    mRegistry
            .view<Bullet, Renderable, HitRadius>()
            .each([&](const auto bulletId, const auto, const auto &bulletRenderable, const auto &bulletHitRadius) {
                if (viewport.contains(bulletRenderable->getPosition())) {
                    const auto terrains = mRegistry.group<Terrain>(entt::get < Renderable, HitRadius > );

                    for (const auto terrainId : terrains) {
                        const auto &[terrainRenderable, terrainHitRadius] = terrains.get<Renderable, HitRadius>(terrainId);
                        if (helpers::magnitude(terrainRenderable->getPosition(), bulletRenderable->getPosition()) <= *terrainHitRadius + *bulletHitRadius) {
                            mRegistry.get<Health>(bulletId).kill();
                            break;
                        }
                    }
                } else {
                    mRegistry.get<Health>(bulletId).kill();
                }
            });

    // player exits screen / player hits terrain
    mRegistry
            .group<Player>(entt::get < Renderable, HitRadius > )
            .each([&](const auto playerId, const auto, auto &playerRenderable, const auto &playerHitRadius) {
                if (viewport.contains(playerRenderable->getPosition())) {
                    const auto terrains = mRegistry.group<Terrain>(entt::get < Renderable, HitRadius > );

                    for (const auto terrainId : terrains) {
                        const auto &[terrainRenderable, terrainHitRadius] = terrains.get<Renderable, HitRadius>(terrainId);
                        if (helpers::magnitude(terrainRenderable->getPosition(), playerRenderable->getPosition()) <= *terrainHitRadius + *playerHitRadius) {
                            assets.getAudioManager().play(SoundId::Explosion);
                            mRegistry.get<Health>(playerId).harm(Damage(1));
                            playerRenderable->setPosition({viewport.width / 2.0f, viewport.height / 4.0f});
                            break;
                        }
                    }
                } else {
                    solarSystemExited = true;
                    playerRenderable->setPosition({viewport.width / 2.0f, viewport.height / 4.0f});
                }
            });

    if (isTractorActive) {
        assets.getAudioManager().play(SoundId::Tractor);
    }

    if (solarSystemExited) {
        const auto bullets = mRegistry.view<Bullet>();
        mRegistry.destroy(bullets.begin(), bullets.end());
        mNextSceneId = mSolarSystemSceneId;
        pubsub::publish<SolarSystemEntered>(window, mRegistry, getSceneId(), mBonus);
    }
}

void PlanetAssault::reloadSystem(sf::Time elapsed) noexcept {
    mRegistry.view<ReloadTime>().each([&](auto &reloadTime) {
        reloadTime.elapse(elapsed);
    });
}

void PlanetAssault::AISystem(Assets &assets) noexcept {
    auto AI1Precision = FloatDistribution(-16.0f, 16.0f);
    auto AI2Precision = FloatDistribution(-8.0f, 8.0f);

    mRegistry.view<Player, Renderable>().each([&](const auto, const auto playerRenderable) {
        mRegistry
                .group<AI1>(entt::get < Renderable, HitRadius, ReloadTime > )
                .each([&](const auto, const auto &AIRenderable, const auto &AIHitRadius, auto &AIReloadTime) {
                    if (AIReloadTime.canShoot()) {
                        const auto bulletRotation = helpers::rotation(AIRenderable->getPosition(), playerRenderable->getPosition()) +
                                                    AI1Precision(mRandomEngine);
                        const auto bulletPosition = AIRenderable->getPosition() + helpers::makeVector2(bulletRotation, *AIHitRadius + 1.0f);
                        AIReloadTime.reset();
                        shoot(mRegistry, assets, bulletPosition, bulletRotation);
                    }
                });

        mRegistry
                .group<AI2>(entt::get < Renderable, HitRadius, ReloadTime > )
                .each([&](const auto, const auto &AIRenderable, const auto &AIHitRadius, auto &AIReloadTime) {
                    if (AIReloadTime.canShoot()) {
                        const auto bulletRotation = helpers::rotation(AIRenderable->getPosition(), playerRenderable->getPosition()) +
                                                    AI2Precision(mRandomEngine);
                        const auto bulletPosition = AIRenderable->getPosition() + helpers::makeVector2(bulletRotation, *AIHitRadius + 1.0f);
                        AIReloadTime.reset();
                        shoot(mRegistry, assets, bulletPosition, bulletRotation);
                    }
                });
    });
}

void PlanetAssault::livenessSystem(Assets &assets) noexcept {
    const auto players = mRegistry.view<Player, Health, Energy>();
    auto entitiesToDestroy = std::vector<entt::entity>();

    mRegistry.view<Health>().each([&](const auto id, const auto &health) {
        if (health.isOver()) {
            entitiesToDestroy.push_back(id);
            if (mRegistry.has<Player>(id) or mRegistry.has<Bunker>(id)) {
                assets.getAudioManager().play(SoundId::Explosion);
            }
        }
    });

    mRegistry.view<Energy>().each([&](const auto id, const auto &energy) {
        if (energy.isOver()) {
            entitiesToDestroy.push_back(id);
        }
    });

    for (const auto id : players) {
        const auto &[health, energy] = players.get<Health, Energy>(id);
        if (health.isOver() or energy.isOver()) {
            mNextSceneId = mLeaderBoardSceneId;
            pubsub::publish<GameOver>(mRegistry.get<Score>(id).value);
            return;
        }
    }

    mRegistry.destroy(entitiesToDestroy.begin(), entitiesToDestroy.end());
}

void PlanetAssault::reportSystem(const sf::RenderWindow &window) noexcept {
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

void shoot(entt::registry &registry, Assets &assets, const sf::Vector2f &position, const float rotation) noexcept {
    auto bulletRenderable = assets.getSpriteSheetsManager().get(SpriteSheetId::Bullet).getSprite({0, 0});
    const auto bulletBounds = bulletRenderable.getLocalBounds();
    const auto bulletId = registry.create();
    static const auto bulletHitRadius = std::max(bulletBounds.width, bulletBounds.height) / 2.0f;

    helpers::centerOrigin(bulletRenderable, bulletBounds);
    bulletRenderable.setRotation(rotation);
    bulletRenderable.setPosition(position);

    registry.assign<Bullet>(bulletId);
    registry.assign<Health>(bulletId, 1);
    registry.assign<Damage>(bulletId, 1);
    registry.assign<HitRadius>(bulletId, bulletHitRadius);
    registry.assign<Renderable>(bulletId, std::move(bulletRenderable));
    registry.assign<Velocity>(bulletId, helpers::makeVector2(rotation, BULLET_SPEED));

    assets.getAudioManager().play(SoundId::Shot);
}

static_assert(TERRAIN_SEGMENTS_PER_UNIT >= 1u);
