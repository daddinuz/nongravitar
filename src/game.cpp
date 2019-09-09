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

Game &Game::initialize() {
    mSpriteSheetsManager.initialize();
    mSpritesManager.initialize();
    mSoundTracksManager.initialize();
    mFontsManager.initialize();
    mAnimationsManager.initialize(mFontsManager);

    mWindow.create({800, 600}, "Gravitar", sf::Style::Fullscreen);
    mWindow.setVerticalSyncEnabled(true);
    mWindow.setFramerateLimit(60);
    mWindow.setActive(true);

    mSoundTracksManager.play(SoundTrackId::MainTheme);
    mScene = Scene::Curtain;

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

        while (mWindow.pollEvent(mEvent)) {
            switch (mEvent.type) {
                case sf::Event::Closed: mWindow.close();
                    break;

                case sf::Event::KeyPressed:
                    switch (mEvent.key.code) {
                        case sf::Keyboard::Delete: mWindow.close();
                            break;

                        case sf::Keyboard::Escape: mWindow.create({800, 600}, "Gravitar", sf::Style::Titlebar | sf::Style::Close);
                            break;

                        case sf::Keyboard::F4: mWindow.create({800, 600}, "Gravitar", sf::Style::Fullscreen);
                            break;

                        case sf::Keyboard::F6: mSoundTracksManager.togglePlaying();
                            break;

                        default: break;
                    }

                default: break;
            }
        }

        update();
    }
}

void Game::initializeSolarSystemScene() {
    using namespace components;

    decltype(auto) player = mRegistry.create();
    decltype(auto) sprite = mSpriteSheetsManager.get(SpriteSheetId::SpaceShip).getSprite({0, 0});
    decltype(auto) bounds = sprite.getLocalBounds();

    sprite.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);

    mRegistry.assign < entt::tag < "player"_hs >> (player);
    mRegistry.assign<Position>(player, 400.0f, 300.0f);
    mRegistry.assign<Velocity>(player);
    mRegistry.assign<Speed>(player, 200.0f);
    mRegistry.assign<RotationSpeed>(player, 360.0f);
    mRegistry.assign<sf::Sprite>(player, std::move(sprite));
}

void Game::updateCurtainScene() {
    decltype(auto) windowSize = mWindow.getSize();
    decltype(auto) title = mSpritesManager.get(SpriteId::GravitarTitle);
    decltype(auto) continueAnimation = mAnimationsManager.get<AnimationId::Continue>();

    title->setPosition({windowSize.x / 2.0f, windowSize.y / 3.14f});

    continueAnimation->setPosition({windowSize.x / 2.0f, windowSize.y / 1.2f});
    continueAnimation.update(mTimer.getElapsedTime());

    mWindow.draw(title);
    mWindow.draw(continueAnimation);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        mScene = Scene::SolarSystem;
    }
}

void Game::updateSolarSystemScene() {
    using namespace components;

    mRegistry.view<Position, Velocity, Speed, RotationSpeed, sf::Sprite>().each([this](auto &position, auto &velocity, const auto &speed, const auto &angularSpeed, auto &sprite) {
        decltype(auto) mousePosition = mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow));
        decltype(auto) rotation = helpers::signum(helpers::shortestRotation(
                sprite.getRotation(),
                helpers::rotation(*position, mousePosition))
        ) * (*angularSpeed) * mTimer.getElapsedTime().asSeconds();

        sprite.rotate(rotation);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            *velocity = helpers::makeVector2(sprite.getRotation(), *speed);
        }

        *position += *velocity * mTimer.getElapsedTime().asSeconds();

        sprite.setPosition(*position);
        mWindow.draw(sprite);
    });
}

void Game::updatePlanetAssaultScene() {
    throw std::runtime_error(trace("unimplemented"));
}
