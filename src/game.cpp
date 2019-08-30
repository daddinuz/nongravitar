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
    window.create({800, 600}, "Gravitar", sf::Style::Fullscreen);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    window.setActive(true);

    if (soundtrack.openFromFile(soundtrack_path("opening.wav"))) { // if fails do nothing
        soundtrack.setLoop(true);
        soundtrack.play();
    }

    state = State::OpeningScreen;

    return *this;
}

void gravitar::Game::run() {
    for (clock.restart(); window.isOpen(); clock.restart()) {
        window.display();

        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed: window.close();
                    break;

                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::F11: window.create({800, 600}, "Gravitar", sf::Style::Fullscreen);
                            break;

                        case sf::Keyboard::Escape: window.create({800, 600}, "Gravitar", sf::Style::Titlebar | sf::Style::Close);
                            break;

                        default: break;
                    }

                default: break;
            }
        }

        window.clear();
    }
}
