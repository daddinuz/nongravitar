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
#include "trace.hpp"
#include "assets.hpp"

gravitar::Game &gravitar::Game::initialize() {
    mAssetsManager.initialize();

    mWindow.create({800, 600}, "Gravitar", sf::Style::Fullscreen);
    mWindow.setVerticalSyncEnabled(true);
    mWindow.setFramerateLimit(60);
    mWindow.setActive(true);

    mAssetsManager.soundtracks().playTitleSoundtrack();
    mView = View::Title;

    return *this;
}

void gravitar::Game::update() {
    switch (mView) {
        case View::Title: updateTitleView();
            break;

        case View::SolarSystem: updateSolarSystemView();
            break;

        case View::PlanetAssault: updatePlanetAssaultView();
            break;
    }
}

void gravitar::Game::run() {
    for (mTimer.restart(); mWindow.isOpen(); mTimer.restart()) {
        update();
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

void gravitar::Game::updateTitleView() {
    const auto windowSize = mWindow.getSize();

    {
        auto title = sf::Text(
                R"(
                    ____           ___________
         ________  / _  \___   ____\__     __/__
  _______\____   \/ /_\  \  \ /   /  /    |/ _  \________
 /  _____/|     _/   |    \  .   /   |    / /_\  \____   \
/   \  ___|  |   \___|____/     /    \___/   |    \     _/
\    \_\  \__|_  /        _____/\___/    \___|__  |  |   \
 _______  /    \/                               \/\__|_  /
        \/                                             \/

        )",
                mAssetsManager.fonts().mechanicalFont(), 16);
        auto textRect = title.getGlobalBounds();

        title.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        title.setPosition({windowSize.x / 2.0f, windowSize.y / 3.14f});

        title.setStyle(sf::Text::Italic);
        title.setOutlineColor(sf::Color::Blue);
        title.setOutlineThickness(4.0f);

        textRect = title.getGlobalBounds();
        auto left = sf::Vector2f{textRect.left + 42.0f, textRect.top + textRect.height};
        auto right = sf::Vector2f{textRect.left - 42.0f + textRect.width, textRect.top + textRect.height};
        auto center = sf::Vector2f{windowSize.x / 2.0f, windowSize.y / 2.0f};
        const auto points = 16;
        const auto diff = (right.x - left.x) / points;
        sf::Vertex line[2] = {center,};
        line->color = sf::Color::Red;

        for (auto i = 0; i < points / 2; i++) {
            line[1] = sf::Vertex({left.x + i * diff, left.y - i * 4.28f});
            mWindow.draw(line, 2, sf::Lines);
        }

        for (auto i = 0; i <= points / 2; i++) {
            line[1] = sf::Vertex({right.x - i * diff, left.y - i * 4.28f});
            mWindow.draw(line, 2, sf::Lines);
        }

        mWindow.draw(title);
    }

    {
        auto playDialog = sf::Text("press [SPACE] to play", mAssetsManager.fonts().mechanicalFont(), 16);
        auto textRect = playDialog.getLocalBounds();

        playDialog.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        playDialog.setPosition({windowSize.x / 2.0f, windowSize.y / 1.5f});

        mWindow.draw(playDialog);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        mView = View::SolarSystem;
    }
}

void gravitar::Game::updateSolarSystemView() {
    throw std::runtime_error(trace("unimplemented"));
}

void gravitar::Game::updatePlanetAssaultView() {
    throw std::runtime_error(trace("unimplemented"));
}
