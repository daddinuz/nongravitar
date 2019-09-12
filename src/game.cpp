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

#include "animation.hpp"
#include "components.hpp"
#include "game.hpp"
#include "helpers.hpp"
#include "trace.hpp"

using namespace gravitar;

inline constexpr float SPEED = 300.0f;
inline constexpr float ROTATION_SPEED = 360.0f;

Game &Game::initialize() {
    mFontsManager.initialize();
    mTexturesManager.initialize();
    mSoundTracksManager.initialize();

    mSpaceLabel.initialize(mFontsManager);
    mGravitarTitle.initialize(mTexturesManager);
    mSpriteSheetsManager.initialize(mTexturesManager);

    mWindow.create({800, 600}, "Gravitar", sf::Style::Fullscreen);
    mWindow.setVerticalSyncEnabled(true);
    mWindow.setFramerateLimit(60);
    mWindow.setActive(true);

    mSoundTracksManager.play(SoundTrackId::MainTheme);

    initializeSolarSystemScene();

    return *this;
}

void Game::update() {
    switch (mScene) {
        case Scene::Curtain: updateCurtainScene();
            break;

        case Scene::SolarSystem: updateSolarSystemScene();
            break;

        case Scene::PlanetAssault: updatePlanetAssaultScene();
            break;
    }
}

void Game::run() {
    for (mTimer.restart(); mWindow.isOpen(); mTimer.restart()) {
        mWindow.display();
        mWindow.clear();
        handleGeneralInputs();
        update();
    }
}

void Game::initializeSolarSystemScene() {
    using namespace components;

    mRegistry.group<Position, Velocity>();
    mRegistry.group<Renderable, Rotation>(entt::get < Position > );

    decltype(auto) player = mRegistry.create();
    decltype(auto) sprite = mSpriteSheetsManager.get(SpriteSheetId::SpaceShip).getSprite({0, 0});
    decltype(auto) bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);

    mRegistry.assign < entt::tag < "player"_hs >> (player);

    mRegistry.assign<Position>(player, 400.0f, 300.0f);
    mRegistry.assign<Velocity>(player);

    mRegistry.assign<Rotation>(player);
    mRegistry.assign<Renderable>(player, std::move(sprite));
}

void Game::handleGeneralInputs() {
    sf::Event event{};

    // flush the events queue (required by SFML in order to work properly)
    while (mWindow.pollEvent(event)) {
        if (sf::Event::Closed == event.type) {
            mWindow.close();
        } else if (sf::Event::KeyPressed == event.type) {
            switch (event.key.code) {
                case sf::Keyboard::Escape: mWindow.close();
                    break;

                case sf::Keyboard::F6: mSoundTracksManager.togglePlaying();
                    break;
#ifndef NDEBUG
                case sf::Keyboard::Delete: mWindow.create({800, 600}, "Gravitar", sf::Style::Close);
                    break;

                case sf::Keyboard::F4: mWindow.create({800, 600}, "Gravitar", sf::Style::Fullscreen);
                    break;
#endif
                default: break;
            }
        }
    }
}

void Game::updateCurtainScene() {
    decltype(auto) windowSize = mWindow.getSize();

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
    renderSystem();
}

void Game::updatePlanetAssaultScene() {
    throw std::runtime_error(trace("unimplemented"));
}

void Game::inputSystem() {
    using namespace components;

    mRegistry.view < entt::tag < "player"_hs > , Position, Velocity, Rotation > ().each([this](const auto tag, const auto &position, auto &velocity, auto &rotation) {
        (void) tag;

        decltype(auto) mousePosition = mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow));
        *rotation += helpers::signum(helpers::shortestRotation(*rotation, helpers::rotation(*position, mousePosition))) * ROTATION_SPEED * mTimer.getElapsedTime().asSeconds();
        *velocity *= 0.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            *velocity += helpers::makeVector2(270.0f, 1.0f);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            *velocity += helpers::makeVector2(180.0f, 1.0f);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            *velocity += helpers::makeVector2(90.0f, 1.0f);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            *velocity += helpers::makeVector2(0.0f, 1.0f);
        }

        *velocity = helpers::normalized({0, 0}, *velocity) * SPEED;
    });
}

void Game::motionSystem() {
    using namespace components;

    mRegistry.group<Position, Velocity>().each([this](auto &position, auto &velocity) {
        *position += *velocity * mTimer.getElapsedTime().asSeconds();
    });
}

void Game::renderSystem() {
    using namespace components;

    mRegistry.group<Renderable, Rotation>(entt::get < Position > ).each([this](auto &renderable, const auto &rotation, const auto &position) {
        std::visit([this, &position, &rotation](auto &drawable) {
            drawable.setPosition(*position);
            drawable.setRotation(*rotation);
            mWindow.draw(drawable);
        }, *renderable);
    });
}
