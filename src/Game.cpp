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
#include <constants.hpp>
#include <helpers.hpp>
#include <Game.hpp>

using namespace gravitar;
using namespace gravitar::constants;

Game::Game() : mRandomEngine(std::random_device()()) {}

Game &Game::initialize() {
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
    mWindow.setFramerateLimit(FPS);
    mWindow.setActive(true);
}

void Game::initializeScenes() {
    // TODO think about deferred scene initialization (initialize method allocs out of ctor)
    auto &youWon = mSceneManager.emplace<scene::YouWon>(mAssets);
    auto &gameOver = mSceneManager.emplace<scene::GameOver>(mAssets);
    auto &solarSystem = mSceneManager.emplace<scene::SolarSystem>(youWon.getSceneId(), gameOver.getSceneId(), mAssets, mRandomEngine);
    auto &titleScreen = mSceneManager.emplace<scene::TitleScreen>(solarSystem.getSceneId(), mAssets);

    solarSystem.addPlayer(mWindow, mAssets);

    for (auto planets = helpers::i_distribution(4, 9)(mRandomEngine), i = 0; i < planets; i++) {
        auto &planetAssault = mSceneManager.emplace<scene::PlanetAssault>(gameOver.getSceneId(), mAssets, mRandomEngine);

        // TODO maybe merge methods
        planetAssault.setParentSceneId(solarSystem.getSceneId());
        planetAssault.initialize(mWindow, mAssets);
        // -------------------------------------------------------

        solarSystem.addPlanet(planetAssault.getSceneId(), mWindow);
        pubsub::subscribe<messages::PlanetEntered>(planetAssault);
    }

    pubsub::subscribe<messages::SolarSystemEntered>(solarSystem);
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

                case sf::Keyboard::Delete:
                    helpers::debug([&]() { mWindow.create({800, 600}, "Gravitar", sf::Style::Close); });
                    break;

                case sf::Keyboard::F4:
                    helpers::debug([&]() { mWindow.create({800, 600}, "Gravitar", sf::Style::Fullscreen); });
                    break;

                default:
                    mSceneId = mSceneManager.get(mSceneId).onEvent(event);
                    break;
            }
        }
    }
}
