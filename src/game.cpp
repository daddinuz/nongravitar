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
#include "cycle.hpp"
#include "game.hpp"
#include "trace.hpp"

gravitar::Game &gravitar::Game::initialize() {
    mSpriteSheetsManager.initialize();
    mSoundtracksManager.initialize();
    mFontsManager.initialize();

    mWindow.create({800, 600}, "Gravitar", sf::Style::Fullscreen);
    mWindow.setVerticalSyncEnabled(true);
    mWindow.setFramerateLimit(60);
    mWindow.setActive(true);

    mSoundtracksManager.play(SoundtrackId::MainTheme);
    mScene = Scene::Curtain;

    return *this;
}

void gravitar::Game::update() {
    switch (mScene) {
        case Scene::Curtain: updateCurtainScene();
            break;

        case Scene::SolarSystem: updateSolarSystemScene();
            break;

        case Scene::PlanetAssault: updatePlanetAssaultScene();
            break;
    }
}

void gravitar::Game::run() {
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

                        case sf::Keyboard::F6: mSoundtracksManager.togglePlaying();
                            break;

                        default: break;
                    }

                default: break;
            }
        }

        update();
    }
}

void gravitar::Game::updateCurtainScene() {
    const auto windowSize = mWindow.getSize();

    {
        auto title = sf::Text(R"(
                    ____           ___________
         ________  / _  \___   ____\__     __/__
  _______\____   \/ /_\  \  \ /   /  /    |/ _  \________
 /  _____/|     _/   |    \  .   /   |    / /_\  \____   \
/   \  ___|  |   \___|____/     /    \___/   |    \     _/
\    \_\  \__|_  /        _____/\___/    \___|__  |  |   \
 _______  /    \/                               \/\__|_  /
        \/                                             \/

)", mFontsManager.get(FontId::Mechanical), 16);
        auto titleRect = title.getGlobalBounds();

        title.setOrigin(titleRect.left + titleRect.width / 2.0f, titleRect.top + titleRect.height / 2.0f);
        title.setPosition({windowSize.x / 2.0f, windowSize.y / 3.14f});

        title.setStyle(sf::Text::Italic);
        title.setFillColor(sf::Color::White);
        title.setOutlineColor(sf::Color(0, 32, 232));
        title.setOutlineThickness(6.0f);

        titleRect = title.getGlobalBounds();
        auto left = sf::Vector2f{titleRect.left + 42.0f, titleRect.top + titleRect.height};
        auto right = sf::Vector2f{titleRect.left - 42.0f + titleRect.width, titleRect.top + titleRect.height};
        auto center = sf::Vector2f{windowSize.x / 2.0f, windowSize.y / 2.0f};
        const auto points = 16;
        const auto diff = (right.x - left.x) / points;
        sf::Vertex line[2] = {center,};
        line->color = sf::Color::Red;

        for (auto i = 0; i < points / 2; i++) {
            line[1] = sf::Vertex({left.x + i * diff, left.y - i * 4.64f});
            mWindow.draw(line, 2, sf::Lines);
        }

        for (auto i = 0; i <= points / 2; i++) {
            line[1] = sf::Vertex({right.x - i * diff, left.y - i * 4.64f});
            mWindow.draw(line, 2, sf::Lines);
        }

        mWindow.draw(title);
    }

    {
        static const std::array<sf::Color, 12> colors = {
                sf::Color(255, 255, 255, 255),
                sf::Color(245, 245, 245, 245),
                sf::Color(235, 235, 235, 235),
                sf::Color(225, 225, 225, 225),
                sf::Color(215, 215, 215, 215),
                sf::Color(205, 205, 205, 205),
                sf::Color(200, 200, 200, 200),
                sf::Color(215, 215, 215, 215),
                sf::Color(225, 225, 225, 225),
                sf::Color(235, 235, 235, 235),
                sf::Color(245, 245, 245, 245),
                sf::Color(255, 255, 255, 255)
        };

        static Animation<sf::Text, Cycle<const std::array<sf::Color, 12>>> space(
                [](auto &delegate) {
                    delegate->setFillColor(*++delegate.frames);
                },
                sf::Text("[SPACE]", mFontsManager.get(FontId::Mechanical), 24),
                Cycle<const std::array<sf::Color, 12>>(colors)
        );

        space.setFramePerSecond(12);

        const auto spaceRect = space->getLocalBounds();
        space->setOrigin(spaceRect.left + spaceRect.width / 2.0f, spaceRect.top + spaceRect.height / 2.0f);
        space->setPosition({windowSize.x / 2.0f, windowSize.y / 1.2f});

        space.update(mTimer.getElapsedTime());
        mWindow.draw(space);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        mScene = Scene::SolarSystem;
    }
}

void gravitar::Game::updateSolarSystemScene() {
    static auto spaceShip = Animation<sf::Sprite, Cycle<SpriteSheet::FrameBuffer>>(
            [](auto &delegate) {
                delegate->setTextureRect(*++delegate.frames);
            },
            mSpriteSheetsManager.get(SpriteSheetId::SpaceShip).getSprite({0, 0}),
            Cycle<SpriteSheet::FrameBuffer>(*mSpriteSheetsManager.get(SpriteSheetId::SpaceShip))
    );

    spaceShip.setFramePerSecond(8);

    const auto spaceShipRect = spaceShip->getLocalBounds();
    spaceShip->setOrigin(spaceShipRect.left + spaceShipRect.width / 2.0f, spaceShipRect.top + spaceShipRect.height / 2.0f);
    spaceShip->setPosition(sf::Vector2f(mWindow.getSize() / 2u));

    spaceShip.update(mTimer.getElapsedTime());
    mWindow.draw(spaceShip);
}

void gravitar::Game::updatePlanetAssaultScene() {
    throw std::runtime_error(trace("unimplemented"));
}
