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

#include <scene/TitleScreen.hpp>
#include <scene/SolarSystem.hpp>
#include <scene/PlanetAssault.hpp>
#include <scene/GameOver.hpp>
#include <scene/YouWon.hpp>
#include <Game.hpp>

using namespace gravitar;

Game &gravitar::Game::initialize() {
    mAssets.initialize();
    initializeWindow();
    initializeScenes();
    return *this;
}

int Game::run() {
    mClock.restart();

    for (handleEvents(); nullSceneId != mSceneId; handleEvents()) {
        mSceneId = mSceneManager.get(mSceneId).update(mWindow, mAssets, mClock.restart());
        mWindow.clear();
        mSceneManager.get(mSceneId).render(mWindow);
        mWindow.display();
    }

    mWindow.close();
    return 0;
}

void Game::initializeWindow() {
    mWindow.create({800, 600}, "Gravitar", sf::Style::Fullscreen);
    mWindow.setVerticalSyncEnabled(true);
    mWindow.setKeyRepeatEnabled(false);
    mWindow.setFramerateLimit(60); // roughly 60 fps
    mWindow.setActive(true);
}

void Game::initializeScenes() {
    auto randomDevice = std::random_device();
    auto randomEngine = std::mt19937(randomDevice());

    auto &youWon = mSceneManager.emplace<scene::YouWon>(mAssets);
    auto &gameOver = mSceneManager.emplace<scene::GameOver>(mAssets);
    auto &solarSystem = mSceneManager.emplace<scene::SolarSystem>(youWon.getSceneId(), gameOver.getSceneId(), mAssets);
    auto &titleScreen = mSceneManager.emplace<scene::TitleScreen>(solarSystem.getSceneId(), mAssets);

    solarSystem.addSpaceShip(mWindow, mAssets);

    // generate a random number of planets in range [4, 8]
    for (auto i = 0; i < std::uniform_int_distribution(4, 8)(randomEngine); i++) {
        auto &planetAssault = mSceneManager.emplace<scene::PlanetAssault>(gameOver.getSceneId(), mAssets);
        planetAssault.setParentSceneId(solarSystem.getSceneId());
        solarSystem.addPlanet(planetAssault.getSceneId(), mWindow, randomEngine);
        pubsub::subscribe<messages::PlanetEntered>(planetAssault);
    }

    pubsub::subscribe<messages::PlanetExited>(solarSystem);
    pubsub::subscribe<messages::PlanetDestroyed>(solarSystem);

    mSceneId = titleScreen.getSceneId();
}

void Game::handleEvents() {
    auto event = sf::Event{};

    while (nullSceneId != mSceneId and mWindow.pollEvent(event)) {
        if (sf::Event::KeyPressed == event.type) {
            switch (event.key.code) {
                case sf::Keyboard::Escape:
                    mSceneId = nullSceneId;
                    break;

                case sf::Keyboard::F6:
                    mAssets.getAudioManager().toggle();
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
                    mSceneId = mSceneManager.get(mSceneId).onEvent(event);
                    break;
            }
        }
    }
}
