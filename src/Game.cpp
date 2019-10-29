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
#include <scene/LeaderBoard.hpp>
#include <constants.hpp>
#include <helpers.hpp>
#include <Game.hpp>

using namespace nongravitar;
using namespace nongravitar::scene;
using namespace nongravitar::constants;

Game::Game() {
    initializeWindow();
    initializeScenes();
}

int Game::run() {
    mClock.restart();

    for (handleEvents(); nullSceneId != mCurrentSceneId; handleEvents()) {
        auto &scene = mSceneManager.getScene(mCurrentSceneId);
        mCurrentSceneId = scene.update(mWindow, mSceneManager, mAssets, mClock.restart());
        mWindow.clear();
        scene.render(mWindow);
        mWindow.display();
    }

    mWindow.close();
    return 0;
}

void Game::initializeWindow() {
    mWindow.create({800, 600}, "NonGravitar", sf::Style::Fullscreen);
    mWindow.setVerticalSyncEnabled(true);
    mWindow.setMouseCursorVisible(false);
    mWindow.setKeyRepeatEnabled(false);
    mWindow.setActive(true);
}

void Game::initializeScenes() {
    const auto &leaderBoard = mSceneManager.emplace<LeaderBoard>(mWindow, mAssets);
    const auto &solarSystem = mSceneManager.emplace<SolarSystem>(mWindow, mAssets, leaderBoard.getSceneId());
    const auto &titleScreen = mSceneManager.emplace<TitleScreen>(mWindow, mAssets, solarSystem.getSceneId());

    mCurrentSceneId = titleScreen.getSceneId();
}

void Game::handleEvents() {
    auto event = sf::Event{};

    while (nullSceneId != mCurrentSceneId and mWindow.pollEvent(event)) {
        if (sf::Event::KeyPressed == event.type) {
            switch (event.key.code) {
                case sf::Keyboard::Escape:
                    mCurrentSceneId = nullSceneId;
                    break;

                case sf::Keyboard::F6:
                    mAssets.getAudioManager().toggle();
                    break;

                case sf::Keyboard::Delete:
                    helpers::debug([&]() { mWindow.create({800, 600}, "NonGravitar", sf::Style::None); });
                    break;

                case sf::Keyboard::F4:
                    helpers::debug([&]() { mWindow.create({800, 600}, "NonGravitar", sf::Style::Fullscreen); });
                    break;

                default:
                    mCurrentSceneId = mSceneManager.getScene(mCurrentSceneId).onEvent(event);
                    break;
            }
        }
    }
}
