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

#include "components.hpp"
#include "game.hpp"
#include "helpers.hpp"
#include "trace.hpp"

using namespace gravitar;

using AI1 = entt::tag<"AI1"_hs>;
using AI2 = entt::tag<"AI2"_hs>;
using Bunker = entt::tag<"bunker"_hs>;
using Bullet = entt::tag<"bullet"_hs>;
using Ground = entt::tag<"ground"_hs>;
using Player = entt::tag<"player"_hs>;
using Planet = entt::tag<"planet"_hs>;
using Zombie = entt::tag<"zombie"_hs>;

constexpr float SPEED = 180.0f;
constexpr float ROTATION_SPEED = 180.0f;

Game &Game::initialize() {
    mFontsManager.initialize();
    mTexturesManager.initialize();
    mSoundTracksManager.initialize();

    mSpaceLabel.initialize(mFontsManager);
    mGravitarTitle.initialize(mTexturesManager);
    mSpriteSheetsManager.initialize(mTexturesManager);

    mWindow.create({800, 600}, "Gravitar", sf::Style::Fullscreen);
    mWindow.setVerticalSyncEnabled(true);
    mWindow.setKeyRepeatEnabled(false);
    mWindow.setFramerateLimit(60); // roughly 60 fps
    mWindow.setActive(true);

    mSoundTracksManager.play(SoundTrackId::MainTheme);

    initializeSolarSystemScene();

    return *this;
}

void Game::update() {
    switch (mScene) {
        case Scene::Curtain:
            updateCurtainScene();
            break;

        case Scene::SolarSystem:
            updateSolarSystemScene();
            break;

        case Scene::PlanetAssault:
            updatePlanetAssaultScene();
            break;
    }
}

void Game::run() {
    for (mTimer.restart(); mWindow.isOpen(); mTimer.restart()) {
        mWindow.display();
        mWindow.clear();

        // flush the events queue (required by SFML in order to work properly) and general input handling
        for (auto event = sf::Event{}; mWindow.pollEvent(event);) {
            if (sf::Event::Closed == event.type) {
                mWindow.close();
            } else if (sf::Event::KeyPressed == event.type) {
                switch (event.key.code) {
                    case sf::Keyboard::Escape:
                        mWindow.close();
                        break;

                    case sf::Keyboard::F6:
                        mSoundTracksManager.togglePlaying();
                        break;
// TODO: remove me
#ifndef NDEBUG
                    case sf::Keyboard::Delete:
                        mWindow.create({800, 600}, "Gravitar", sf::Style::Close);
                        break;

                    case sf::Keyboard::F4:
                        mWindow.create({800, 600}, "Gravitar", sf::Style::Fullscreen);
                        break;
#endif
                    default:
                        break;
                }
            }
        }

        update();
    }
}

void Game::initializeSolarSystemScene() {
    using namespace components;

    mRegistry.group<Renderable, Velocity>();

    auto player = mRegistry.create();
    auto renderable = mSpriteSheetsManager.get(SpriteSheetId::SpaceShip).instanceSprite({0, 0});

    helpers::centerOrigin(renderable, renderable.getLocalBounds());
    renderable.setPosition(400.0f, 300.0f);

    mRegistry.assign<Player>(player);
    mRegistry.assign<Health>(player, 3);
    mRegistry.assign<Fuel>(player, 20000);
    mRegistry.assign<Velocity>(player);
    mRegistry.assign<RechargeTime>(player, RechargeTime(1.0f));
    mRegistry.assign<Renderable>(player, std::move(renderable));
}

void Game::updateCurtainScene() {
    const auto windowSize = mWindow.getSize();

    mGravitarTitle.setPosition(windowSize.x / 2.0f, windowSize.y / 3.14f);
    mSpaceLabel.setPosition(windowSize.x / 2.0f, windowSize.y / 1.2f);
    mSpaceLabel.update(mTimer.getElapsedTime());

    mWindow.draw(mGravitarTitle);
    mWindow.draw(mSpaceLabel);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        mScene = Scene::SolarSystem;
    }
}

void Game::updateSolarSystemScene() {
    inputSystem();
    motionSystem();
    collisionSystem();
    renderSystem();
}

void Game::updatePlanetAssaultScene() {
    throw std::runtime_error(trace("unimplemented"));
}

void Game::inputSystem() {
    using namespace components;

    mRegistry.view<Player, Renderable, Velocity, Fuel, RechargeTime>().each([&](const auto &player, auto &renderable, auto &velocity, auto &fuel, auto &rechargeTime) {
        (void) player;
        auto speed = SPEED;
        const auto input = (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ? 1 : 0) + (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ? 2 : 0) +
                           (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ? 4 : 0) + (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ? 8 : 0);

        switch (input) {
            case 0: {
                const auto mousePosition = mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow));
                const auto mouseRotation = helpers::rotation(renderable.getPosition(), mousePosition);
                const auto shortestRotation = helpers::shortestRotation(renderable.getRotation(), mouseRotation);
                renderable.rotate(static_cast<float>(helpers::signum(shortestRotation)) * ROTATION_SPEED * mTimer.getElapsedTime().asSeconds());
            }
                break;

            case 1:
                renderable.rotate(-ROTATION_SPEED * mTimer.getElapsedTime().asSeconds());
                break;

            case 2:
                renderable.rotate(ROTATION_SPEED * mTimer.getElapsedTime().asSeconds());
                break;

            case 4:
                speed *= 1.56f;
                break;

            case 5:
                renderable.rotate(-ROTATION_SPEED * 0.92f * mTimer.getElapsedTime().asSeconds());
                speed *= 1.32f;
                break;

            case 6:
                renderable.rotate(ROTATION_SPEED * 0.92f * mTimer.getElapsedTime().asSeconds());
                speed *= 1.32f;
                break;

            case 8:
                speed *= 0.98f;
                break;

            case 9:
                renderable.rotate(-ROTATION_SPEED * 1.08f * mTimer.getElapsedTime().asSeconds());
                speed *= 0.68f;
                break;

            case 10:
                renderable.rotate(ROTATION_SPEED * 1.08f * mTimer.getElapsedTime().asSeconds());
                speed *= 0.68f;
                break;

            default:
                break;
        }

        *velocity = helpers::makeVector2(renderable.getRotation(), speed);
        *fuel -= speed * mTimer.getElapsedTime().asSeconds();
        rechargeTime.elapse(mTimer.getElapsedTime());

        if (rechargeTime.canShoot() and sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            rechargeTime.reset();
            const auto bulletEntity = mRegistry.create();
            auto bulletRenderable = mSpriteSheetsManager.get(SpriteSheetId::Bullet).instanceSprite({0, 0});

            helpers::centerOrigin(bulletRenderable, bulletRenderable.getLocalBounds());
            bulletRenderable.setPosition(renderable.getPosition() + helpers::makeVector2(renderable.getRotation(), 22.8f));

            mRegistry.assign<Bullet>(bulletEntity);
            mRegistry.assign<Velocity>(bulletEntity, helpers::makeVector2(renderable.getRotation(), 800.0f));
            mRegistry.assign<Renderable>(bulletEntity, std::move(bulletRenderable)); // this must be the last line in order to avoid dangling pointers
        }
    });
}

void Game::motionSystem() {
    using namespace components;

    mRegistry.group<Renderable, Velocity>().each([this](auto &renderable, const auto &velocity) {
        renderable.move(*velocity * mTimer.getElapsedTime().asSeconds());
    });
}

void Game::collisionSystem() {
    using namespace components;

    const auto viewport = sf::FloatRect(mWindow.getViewport(mWindow.getView()));

    mRegistry.view<Player, Renderable>().each([&](const auto &entity, const auto &player, auto &renderable) {
        (void) player;

        if (not viewport.intersects(renderable.getHitBox())) {
            *mRegistry.get<Health>(entity) -= 1;
            renderable.setPosition({viewport.width / 2, viewport.height / 2});
        }
    });

    mRegistry.view<Bullet, Renderable>().each([&](const auto &bulletEntity, const auto &bullet, const auto &bulletRenderable) {
        (void) bullet;

        if (not viewport.intersects(bulletRenderable.getHitBox())) {
            mRegistry.destroy(bulletEntity);
        } else {
            mRegistry.view<Player, Renderable>().each([&](const auto &playerEntity, const auto &player, auto &playerRenderable) {
                (void) player;

                if (bulletRenderable.getHitBox().intersects(playerRenderable.getHitBox())) {
                    mRegistry.destroy(bulletEntity);
                    *mRegistry.get<Health>(playerEntity) -= 1;
                }
            });
        }
    });
}

void Game::renderSystem() {
    using namespace components;

    mRegistry.view<Renderable>().each([this](const auto &renderable) {
        helpers::debug([&]() { // display hit-box on debug builds only
            const auto hitBox = renderable.getHitBox();
            auto shape = sf::RectangleShape({hitBox.width, hitBox.height});
            shape.setPosition({hitBox.left, hitBox.top});
            shape.setFillColor(sf::Color::Transparent);
            shape.setOutlineColor(sf::Color::Red);
            shape.setOutlineThickness(1);
            mWindow.draw(shape);
        });

        mWindow.draw(renderable);
    });
}
