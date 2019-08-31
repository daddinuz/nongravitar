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

#include "game.hpp"
#include "assets.hpp"

gravitar::Game &gravitar::Game::initialize() {
    mAssetsManager.initialize();

    mWindow.create({800, 600}, "Gravitar", sf::Style::Fullscreen);
    mWindow.setVerticalSyncEnabled(true);
    mWindow.setFramerateLimit(60);
    mWindow.setActive(true);

    mAssetsManager.soundtracks().playTitleSoundtrack();
    mState = State::TitleScreen;

    return *this;
}

void gravitar::Game::run() {
    for (mClock.restart(); mWindow.isOpen(); mClock.restart()) {
        mWindow.display();

        while (mWindow.pollEvent(mEvent)) {
            switch (mEvent.type) {
                case sf::Event::Closed: mWindow.close();
                    break;

                case sf::Event::KeyPressed:
                    switch (mEvent.key.code) {
                        case sf::Keyboard::Escape: mWindow.create({800, 600}, "Gravitar", sf::Style::Titlebar | sf::Style::Close);
                            break;

                        case sf::Keyboard::F4: mWindow.create({800, 600}, "Gravitar", sf::Style::Fullscreen);
                            break;

                        case sf::Keyboard::F6: mAssetsManager.soundtracks().togglePlaying();
                            break;

                        default: break;
                    }

                default: break;
            }
        }

        mWindow.clear();
    }
}
