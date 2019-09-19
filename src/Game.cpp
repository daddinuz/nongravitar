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
#include <scene/GameOver.hpp>
#include <Game.hpp>

using namespace gravitar;

Game &gravitar::Game::initialize() {
    mAssetsManager.initialize();
    initializeWindow();
    initializeScenes();
    return *this;
}

int Game::run() {
    mClock.restart();

    for (handleEvents(); scene::NullScene != mSceneId; handleEvents()) {
        auto &scene = mSceneManager.get(mSceneId);
        scene.update(mWindow, mAssetsManager, mClock.restart());
        mWindow.clear();
        scene.render(mWindow);
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
    // auto gameOverScene = mSceneManager.emplace<scene::GameOver>(mFontManager);
    auto solarSystemScene = mSceneManager.emplace<scene::SolarSystem>(mAssetsManager);
    mSceneId = mSceneManager.emplace<scene::TitleScreen>(solarSystemScene, mAssetsManager);
}

void Game::handleEvents() {
    auto event = sf::Event{};

    while (scene::NullScene != mSceneId and mWindow.pollEvent(event)) {
        if (sf::Event::KeyPressed == event.type) {
            switch (event.key.code) {
                case sf::Keyboard::Escape:
                    mSceneId = scene::NullScene;
                    break;

                case sf::Keyboard::F6:
                    mAssetsManager.getAudioManager().toggle();
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
