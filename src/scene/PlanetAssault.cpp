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
#include <Canvas.hpp>
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

constexpr auto BULLET_SPAWN_OFFSET = 8.0f;
constexpr auto TERRAIN_SEGMENTS_PER_UNIT = 4u;

struct SignalHandlers {
    void playExplosion(const entt::entity, const entt::registry &) {
        assets.getAudioManager().play(SoundId::Explosion);
    }

    void publishScore(const entt::entity id, const entt::registry &registry) {
        (void) assets; // Note: this method cannot be static due to entt sink requirements
        pubsub::publish<GameOver>(registry.get<Score>(id).value);
    }

    Assets &assets;
};

void shoot(entt::registry &registry, Assets &assets, const sf::Vector2f &position, float rotation);

PlanetAssault::PlanetAssault(const SceneId solarSystemSceneId, const SceneId leaderBoardSceneId, const sf::Color terrainColor) :
        mBuffer{}, mRandomEngine{RandomDevice()()}, mLeaderBoardSceneId{leaderBoardSceneId}, mSolarSystemSceneId{solarSystemSceneId}, mTerrainColor(terrainColor) {}

SceneId PlanetAssault::update(const sf::RenderWindow &window, SceneManager &, Assets &assets, const sf::Time elapsed) {
    mNextSceneId = getSceneId();

    if (auto &audioManager = assets.getAudioManager(); SoundTrackId::ComputerAdventures != audioManager.getPlaying()) {
        audioManager.play(SoundTrackId::ComputerAdventures);
    }

    inputSystem(assets, elapsed);
    motionSystem(elapsed);
    collisionSystem(window, assets, elapsed);
    reloadSystem(elapsed);
    AISystem(assets);
    livenessSystem();
    reportSystem(window);

    return mNextSceneId;
}

void PlanetAssault::render(sf::RenderTarget &window) const {
    auto canvas = Canvas();

    mRegistry
            .group<>(entt::get<const Transformation, const Sprite>, entt::exclude<const Color>)
            .each([&](const auto &transformable, const auto &sprite) {
                canvas.bind(sprite);
                window.draw(canvas, transformable.getTransform());
            });

    mRegistry
            .group<const Transformation, const Sprite, const Color>()
            .each([&](const auto &transformable, const auto &sprite, const auto &color) {
                canvas.setColor(color);
                canvas.bind(sprite);
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

Scene &PlanetAssault::setup(const sf::RenderWindow &window, nongravitar::Assets &assets) {
    auto &signalHandler = mRegistry.set<SignalHandlers>(assets);
    mRegistry.on_destroy<Score>().connect<&SignalHandlers::publishScore>(signalHandler);
    mRegistry.on_destroy<Bunker>().connect<&SignalHandlers::playExplosion>(signalHandler);
    mRegistry.on_destroy<Player>().connect<&SignalHandlers::playExplosion>(signalHandler);

    mReport.setFont(assets.getFontsManager().getFont(FontId::Mechanical));
    mReport.setFillColor(sf::Color(105, 235, 245, 255));
    mReport.setCharacterSize(18.0f);

    initializeGroups();
    initializeTerrain(window, assets, mTerrainColor);

    pubsub::subscribe<messages::PlanetEntered>(*this);
    return Scene::setup(window, assets);
}

void PlanetAssault::operator()(const PlanetEntered &message) {
    if (message.sceneId == getSceneId()) {
        const auto players = mRegistry.view<Player>();
        const auto tractors = mRegistry.view<Tractor>();

        mRegistry.destroy(players.begin(), players.end());
        mRegistry.destroy(tractors.begin(), tractors.end());

        for (const auto sourcePlayerId : message.registry.view<Player>()) {
            const auto[windowWidth, windowHeight] = sf::Vector2f(message.window.getSize());
            const auto tractorId = mRegistry.create();
            const auto playerId = mRegistry.create(sourcePlayerId, message.registry);

            // player
            auto &playerTransformation = mRegistry.get<Transformation>(playerId);
            playerTransformation.setRotation(90.0f);
            playerTransformation.setPosition(windowWidth / 2.0f, windowHeight / 4.0f);
            mRegistry.assign<EntityRef<Tractor>>(playerId, tractorId);

            // tractor
            mRegistry.assign<Tractor>(tractorId);
            mRegistry.assign<EntityRef<Player>>(tractorId, playerId);
        }
    }
}

void PlanetAssault::initializeGroups() {
    // render
    mRegistry.group<const Transformation, const Sprite, const Color>();
    mRegistry.group<>(entt::get<const Transformation, const Sprite>, entt::exclude<const Color>);

    // motionSystem
    mRegistry.group<Velocity>(entt::get < Transformation > );

    // collisionSystem
    mRegistry.group<Health>(entt::get < Transformation, HitRadius > );
    mRegistry.group<Damage>(entt::get < Transformation, HitRadius > );

    mRegistry.group<Player>(entt::get < Transformation, HitRadius > );
    mRegistry.group<Terrain>(entt::get < Transformation, HitRadius > );
    mRegistry.group<Tractor>(entt::get < Transformation, HitRadius, EntityRef<Player>>);
    mRegistry.group<Bullet>(entt::get < Transformation, HitRadius > );
    mRegistry.group<Supply<Energy>>(entt::get < Transformation, HitRadius > );
    mRegistry.group<Supply<Health>>(entt::get < Transformation, HitRadius > );

    // AISystem
    mRegistry.group<AI1>(entt::get < Transformation, HitRadius, ReloadTime > );
    mRegistry.group<AI2>(entt::get < Transformation, HitRadius, ReloadTime > );
}

void PlanetAssault::initializeTerrain(const sf::RenderWindow &window, Assets &assets, const sf::Color terrainColor) {
    const auto halfWindowHeight = window.getSize().y / 2.0f;
    const auto viewport = sf::FloatRect(window.getViewport(window.getView()));

    const auto terrainHitRadius = assets.getSpriteSheetsManager().getSprite(SpriteSheetId::Terrain, 0).getRadius();
    const auto terrainHitDiameter = terrainHitRadius * 2.0f;
    auto rotationDistribution = FloatDistribution(-32.0f, 32.0f);
    auto terrainPosition = sf::Vector2f(
            0.0f,
            FloatDistribution(halfWindowHeight * 1.5f + terrainHitDiameter, halfWindowHeight * 2.0f - terrainHitDiameter)(mRandomEngine)
    );

    do {
        const auto terrainRotation = rotationDistribution(mRandomEngine);

        for (auto i = 0u; i < TERRAIN_SEGMENTS_PER_UNIT; i++) {
            const auto terrainId = mRegistry.create();
            const auto terrainOffset = helpers::makeVector2(terrainRotation, terrainHitRadius);
            const auto &terrainSprite = mRegistry.assign<Sprite>(terrainId, assets.getSpriteSheetsManager().getSprite(SpriteSheetId::Terrain, 0));
            auto &terrainTransformation = mRegistry.assign<Transformation>(terrainId);

            terrainTransformation.setOrigin(sf::Vector2f(terrainSprite.getSize()) / 2.0f);

            terrainPosition += terrainOffset;
            terrainTransformation.setPosition(terrainPosition);
            terrainTransformation.setRotation(terrainRotation);
            terrainPosition += terrainOffset;

            mRegistry.assign<Terrain>(terrainId);
            mRegistry.assign<Color>(terrainId, terrainColor);
            mRegistry.assign<HitRadius>(terrainId, terrainHitRadius);
        }
    } while (viewport.contains(terrainPosition));

    auto AI1ReloadDistribution = FloatDistribution(1.64f, 2.28f);
    auto AI2ReloadDistribution = FloatDistribution(1.96f, 2.28f);
    auto energySupplyDistribution = FloatDistribution(2000.0f, 4000.0f);
    auto entityDistribution = IntDistribution(1, 16);

    const auto terrains = mRegistry.view<Terrain, Transformation>();
    for (auto terrainCursor = terrains.begin();
         std::distance(terrainCursor, terrains.end()) > TERRAIN_SEGMENTS_PER_UNIT;
         std::advance(terrainCursor, TERRAIN_SEGMENTS_PER_UNIT)) {
        const auto &terrainTransformation = terrains.get<Transformation>(*terrainCursor);
        const auto position = terrainTransformation.getPosition() +
                              helpers::makeVector2(terrainTransformation.getRotation() + 180.0f, terrainHitRadius * (TERRAIN_SEGMENTS_PER_UNIT - 1u));

        switch (entityDistribution(mRandomEngine)) {
            case 2:
            case 16: {
                const auto bunkerId = mRegistry.create();
                const auto &bunkerSprite = mRegistry.assign<Sprite>(bunkerId, assets.getSpriteSheetsManager().getSprite(SpriteSheetId::Bunker, 0));
                auto &bunkerTransformation = mRegistry.assign<Transformation>(bunkerId);

                bunkerTransformation.setOrigin(sf::Vector2f(bunkerSprite.getSize()) / 2.0f);
                bunkerTransformation.setRotation(terrainTransformation.getRotation() + 180.0f);
                bunkerTransformation.setPosition(position + helpers::makeVector2(terrainTransformation.getRotation() + 270.0f, bunkerSprite.getRadius()));

                mRegistry.assign<AI1>(bunkerId);
                mRegistry.assign<Bunker>(bunkerId);
                mRegistry.assign<Damage>(bunkerId, 2);
                mRegistry.assign<Health>(bunkerId, 1);
                mRegistry.assign<HitRadius>(bunkerId, bunkerSprite.getRadius());
                mRegistry.assign<ReloadTime>(bunkerId, AI1ReloadDistribution(mRandomEngine));
            }
                break;

            case 8: {
                const auto bunkerId = mRegistry.create();
                const auto &bunkerSprite = mRegistry.assign<Sprite>(bunkerId, assets.getSpriteSheetsManager().getSprite(SpriteSheetId::Bunker, 1));
                auto &bunkerTransformation = mRegistry.assign<Transformation>(bunkerId);

                bunkerTransformation.setOrigin(sf::Vector2f(bunkerSprite.getSize()) / 2.0f);
                bunkerTransformation.setRotation(terrainTransformation.getRotation() + 180.0f);
                bunkerTransformation.setPosition(position + helpers::makeVector2(terrainTransformation.getRotation() + 270.0f, bunkerSprite.getRadius()));

                mRegistry.assign<AI2>(bunkerId);
                mRegistry.assign<Bunker>(bunkerId);
                mRegistry.assign<Damage>(bunkerId, 3);
                mRegistry.assign<Health>(bunkerId, 2);
                mRegistry.assign<HitRadius>(bunkerId, bunkerSprite.getRadius());
                mRegistry.assign<ReloadTime>(bunkerId, AI2ReloadDistribution(mRandomEngine));
            }
                break;

            case 4: {
                const auto supplyId = mRegistry.create();
                const auto &supplySprite = mRegistry.assign<Sprite>(supplyId, assets.getSpriteSheetsManager().getSprite(SpriteSheetId::Supply, 1));
                auto &supplyTransformation = mRegistry.assign<Transformation>(supplyId);

                supplyTransformation.setOrigin(sf::Vector2f(supplySprite.getSize()) / 2.0f);
                supplyTransformation.setRotation(terrainTransformation.getRotation() + 180.0f);
                supplyTransformation.setPosition(position + helpers::makeVector2(terrainTransformation.getRotation() + 270.0f, supplySprite.getRadius()));

                mRegistry.assign<Damage>(supplyId, 1);
                mRegistry.assign<Health>(supplyId, 1);
                mRegistry.assign<HitRadius>(supplyId, supplySprite.getRadius());
                mRegistry.assign<Supply<Energy>>(supplyId, energySupplyDistribution(mRandomEngine));
            }
                break;

            case 6:
            case 12: {
                const auto supplyId = mRegistry.create();
                const auto &supplySprite = mRegistry.assign<Sprite>(supplyId, assets.getSpriteSheetsManager().getSprite(SpriteSheetId::Supply, 0));
                auto &supplyTransformation = mRegistry.assign<Transformation>(supplyId);

                supplyTransformation.setOrigin(sf::Vector2f(supplySprite.getSize()) / 2.0f);
                supplyTransformation.setRotation(terrainTransformation.getRotation() + 180.0f);
                supplyTransformation.setPosition(position + helpers::makeVector2(terrainTransformation.getRotation() + 270.0f, supplySprite.getRadius()));

                mRegistry.assign<Damage>(supplyId, 1);
                mRegistry.assign<Health>(supplyId, 1);
                mRegistry.assign<HitRadius>(supplyId, supplySprite.getRadius());
                mRegistry.assign<Supply<Health>>(supplyId, 1);
            }
                break;

            default:
                break;
        }
    }

    mBonus += SCORE_PER_AI1 * std::distance(mRegistry.view<AI1>().begin(), mRegistry.view<AI1>().end());
    mBonus += SCORE_PER_AI2 * std::distance(mRegistry.view<AI2>().begin(), mRegistry.view<AI2>().end());
}

void PlanetAssault::inputSystem(Assets &assets, const sf::Time elapsed) {
    using Key = sf::Keyboard::Key;
    const auto isKeyPressed = &sf::Keyboard::isKeyPressed;

    mRegistry
            .view<Player, HitRadius, EntityRef<Tractor>, Transformation, Energy, Velocity, ReloadTime>()
            .each([&](const auto, const auto &playerHitRadius, const auto &playerTractorRef, auto &playerTransformation,
                      auto &playerEnergy, auto &playerVelocity, auto &playerReloadTime) {
                const auto tractorId = *playerTractorRef;
                const auto speed = isKeyPressed(Key::W) ? PLAYER_SPEED_FAST : isKeyPressed(Key::S) ? PLAYER_SPEED_SLOW : PLAYER_SPEED_DEFAULT;
                const auto rotationSign = isKeyPressed(Key::A) ? -1.0f : isKeyPressed(Key::D) ? 1.0f : 0.0f;

                playerTransformation.rotate(rotationSign * PLAYER_ROTATION_SPEED * elapsed.asSeconds());
                playerVelocity.value = helpers::makeVector2(playerTransformation.getRotation(), speed);
                playerEnergy.consume(speed * elapsed.asSeconds());

                if (isKeyPressed(Key::RShift)) {
                    const auto &tractorSprite = mRegistry.assign_or_replace<Sprite>(tractorId, assets.getSpriteSheetsManager().getSprite(SpriteSheetId::Tractor, 0));
                    auto &tractorTransformation = mRegistry.assign_or_replace<Transformation>(tractorId);

                    tractorTransformation.setOrigin(sf::Vector2f(tractorSprite.getSize()) / 2.0f);
                    tractorTransformation.setPosition(playerTransformation.getPosition());

                    mRegistry.assign_or_replace<HitRadius>(tractorId, tractorSprite.getRadius());
                } else {
                    mRegistry.reset<Transformation>(tractorId);
                    mRegistry.reset<HitRadius>(tractorId);
                    mRegistry.reset<Sprite>(tractorId);

                    if (playerReloadTime.canShoot() and isKeyPressed(Key::Space)) {
                        const auto bulletRotation = playerTransformation.getRotation();
                        const auto bulletPosition = playerTransformation.getPosition() + helpers::makeVector2(bulletRotation, *playerHitRadius + BULLET_SPAWN_OFFSET);
                        playerReloadTime.reset();
                        // NOTE for a future me: be aware that this invalidates some component references !!!
                        shoot(mRegistry, assets, bulletPosition, bulletRotation);
                    }
                }
            });
}

void PlanetAssault::motionSystem(const sf::Time elapsed) {
    mRegistry.group<Velocity>(entt::get < Transformation > ).each([&](const auto &velocity, auto &transformable) {
        transformable.move(velocity.value * elapsed.asSeconds());
    });
}

void PlanetAssault::collisionSystem(const sf::RenderWindow &window, Assets &assets, const sf::Time elapsed) {
    const auto viewport = sf::FloatRect(window.getViewport(window.getView()));
    auto solarSystemExited = false;
    auto isTractorActive = false;

    // general entities collisions
    const auto g1 = mRegistry.group<Health>(entt::get < Transformation, HitRadius > );
    const auto g2 = mRegistry.group<Damage>(entt::get < Transformation, HitRadius > );
    for (auto e1 : g1) {
        for (auto e2 : g2) {
            if (e1 != e2) {
                const auto &[entityTransformation1, entityHitRadius1] = g1.get<Transformation, HitRadius>(e1);
                const auto &[entityTransformation2, entityHitRadius2] = g2.get<Transformation, HitRadius>(e2);

                if (helpers::magnitude(entityTransformation1.getPosition(), entityTransformation2.getPosition()) <= *entityHitRadius1 + *entityHitRadius2) {
                    assets.getAudioManager().play(SoundId::Hit);
                    g1.get<Health>(e1).harm(g2.get<Damage>(e2));
                }
            }
        }
    }

    // tractor hits supply
    mRegistry
            .group<Tractor>(entt::get < Transformation, HitRadius, EntityRef<Player>>)
    .each([&](const auto, const auto &tractorTransformation, const auto &tractorHitRadius, const auto &playerRef) {
        const auto playerId = *playerRef;

        mRegistry
                .group<Bullet>(entt::get < Transformation, HitRadius > )
                .each([&](const auto bulletId, const auto, auto &bulletTransformation, const auto &bulletHitRadius) {
                    if (helpers::magnitude(tractorTransformation.getPosition(), bulletTransformation.getPosition()) <= *tractorHitRadius + *bulletHitRadius) {
                        const auto rotationDiff = helpers::shortestRotation(
                                bulletTransformation.getRotation(),
                                helpers::rotation(bulletTransformation.getPosition(), tractorTransformation.getPosition())
                        );

                        bulletTransformation.rotate(helpers::signum(rotationDiff) * 220.0f * elapsed.asSeconds());

                        if (auto velocity = mRegistry.try_get<Velocity>(bulletId); velocity) {
                            velocity->value = helpers::makeVector2(bulletTransformation.getRotation(), BULLET_SPEED);
                        }
                    }
                });

        mRegistry
                .group<Supply<Energy>>(entt::get < Transformation, HitRadius > )
                .each([&](const auto supplyId, const auto &supply, const auto &supplyTransformation, const auto &supplyHitRadius) {
                    if (helpers::magnitude(tractorTransformation.getPosition(), supplyTransformation.getPosition()) <= *tractorHitRadius + *supplyHitRadius) {
                        mRegistry.get<Health>(supplyId).kill();
                        mRegistry.get<Energy>(playerId).recharge(supply);
                        isTractorActive = true;
                    }
                });

        mRegistry
                .group<Supply<Health>>(entt::get < Transformation, HitRadius > )
                .each([&](const auto supplyId, const auto &supply, const auto &supplyTransformation, const auto &supplyHitRadius) {
                    if (helpers::magnitude(tractorTransformation.getPosition(), supplyTransformation.getPosition()) <= *tractorHitRadius + *supplyHitRadius) {
                        mRegistry.get<Health>(supplyId).kill();
                        mRegistry.get<Health>(playerId).heal(supply);
                        isTractorActive = true;
                    }
                });
    });

    // bullet exits screen / bullet hits terrain
    mRegistry
            .group<Bullet>(entt::get < Transformation, HitRadius > )
            .each([&](const auto bulletId, const auto, const auto &bulletTransformation, const auto &bulletHitRadius) {
                if (viewport.contains(bulletTransformation.getPosition())) {
                    const auto terrains = mRegistry.group<Terrain>(entt::get < Transformation, HitRadius > );

                    for (const auto terrainId : terrains) {
                        const auto &[terrainTransformation, terrainHitRadius] = terrains.get<Transformation, HitRadius>(terrainId);
                        if (helpers::magnitude(terrainTransformation.getPosition(), bulletTransformation.getPosition()) <= *terrainHitRadius + *bulletHitRadius) {
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
            .group<Player>(entt::get < Transformation, HitRadius > )
            .each([&](const auto playerId, const auto, auto &playerTransformation, const auto &playerHitRadius) {
                if (viewport.contains(playerTransformation.getPosition())) {
                    const auto terrains = mRegistry.group<Terrain>(entt::get < Transformation, HitRadius > );

                    for (const auto terrainId : terrains) {
                        const auto &[terrainTransformation, terrainHitRadius] = terrains.get<Transformation, HitRadius>(terrainId);
                        if (helpers::magnitude(terrainTransformation.getPosition(), playerTransformation.getPosition()) <= *terrainHitRadius + *playerHitRadius) {
                            assets.getAudioManager().play(SoundId::Explosion);
                            mRegistry.get<Health>(playerId).harm(Damage(1));
                            playerTransformation.setPosition({viewport.width / 2.0f, viewport.height / 4.0f});
                            break;
                        }
                    }
                } else {
                    solarSystemExited = true;
                    playerTransformation.setPosition({viewport.width / 2.0f, viewport.height / 4.0f});
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

void PlanetAssault::reloadSystem(sf::Time elapsed) {
    mRegistry.view<ReloadTime>().each([&](auto &reloadTime) {
        reloadTime.elapse(elapsed);
    });
}

void PlanetAssault::AISystem(Assets &assets) {
    auto AI1Precision = FloatDistribution(-16.0f, 16.0f);
    auto AI2Precision = FloatDistribution(-8.0f, 8.0f);

    mRegistry.view<Player, Transformation>().each([&](const auto, const auto playerTransformation) {
        mRegistry
                .group<AI1>(entt::get < Transformation, HitRadius, ReloadTime > )
                .each([&](const auto, const auto &AITransformation, const auto &AIHitRadius, auto &AIReloadTime) {
                    if (AIReloadTime.canShoot()) {
                        const auto bulletRotation = helpers::rotation(AITransformation.getPosition(), playerTransformation.getPosition()) +
                                                    AI1Precision(mRandomEngine);
                        const auto bulletPosition = AITransformation.getPosition() + helpers::makeVector2(bulletRotation, *AIHitRadius + BULLET_SPAWN_OFFSET);

                        AIReloadTime.reset();
                        shoot(mRegistry, assets, bulletPosition, bulletRotation);
                    }
                });

        mRegistry
                .group<AI2>(entt::get < Transformation, HitRadius, ReloadTime > )
                .each([&](const auto, const auto &AITransformation, const auto &AIHitRadius, auto &AIReloadTime) {
                    if (AIReloadTime.canShoot()) {
                        const auto bulletRotation = helpers::rotation(AITransformation.getPosition(), playerTransformation.getPosition()) +
                                                    AI2Precision(mRandomEngine);
                        const auto bulletPosition = AITransformation.getPosition() + helpers::makeVector2(bulletRotation, *AIHitRadius + BULLET_SPAWN_OFFSET);
                        AIReloadTime.reset();
                        shoot(mRegistry, assets, bulletPosition, bulletRotation);
                    }
                });
    });
}

void PlanetAssault::livenessSystem() {
    auto entitiesToDestroy = std::vector<entt::entity>();

    mRegistry.view<Health>().each([&](const auto id, const auto &health) {
        if (health.isOver()) {
            entitiesToDestroy.push_back(id);
        }
    });

    mRegistry.view<Energy>().each([&](const auto id, const auto &energy) {
        if (energy.isOver()) {
            entitiesToDestroy.push_back(id);
        }
    });

    mRegistry.destroy(entitiesToDestroy.begin(), entitiesToDestroy.end());

    if (mRegistry.view<Player>().begin() == mRegistry.view<Player>().end()) {
        mNextSceneId = mLeaderBoardSceneId;
    }
}

void PlanetAssault::reportSystem(const sf::RenderWindow &window) {
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

void shoot(entt::registry &registry, Assets &assets, const sf::Vector2f &position, const float rotation) {
    const auto bulletId = registry.create();
    const auto &bulletSprite = registry.assign<Sprite>(bulletId, assets.getSpriteSheetsManager().getSprite(SpriteSheetId::Bullet, 0));
    auto &bulletTransformation = registry.assign<Transformation>(bulletId);

    bulletTransformation.setOrigin(sf::Vector2f(bulletSprite.getSize()) / 2.0f);
    bulletTransformation.setRotation(rotation);
    bulletTransformation.setPosition(position);

    registry.assign<Bullet>(bulletId);
    registry.assign<Health>(bulletId, 1);
    registry.assign<Damage>(bulletId, 1);
    registry.assign<HitRadius>(bulletId, bulletSprite.getRadius());
    registry.assign<Velocity>(bulletId, helpers::makeVector2(rotation, BULLET_SPEED));

    assets.getAudioManager().play(SoundId::Shot);
}

static_assert(TERRAIN_SEGMENTS_PER_UNIT >= 1u);
