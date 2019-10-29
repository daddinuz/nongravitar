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

#include <helpers.hpp>
#include <scene/LeaderBoard.hpp>

using namespace nongravitar;
using namespace nongravitar::scene;
using namespace nongravitar::assets;

constexpr auto TOP_PADDING = 64.0f;
constexpr auto BOTTOM_PADDING = 64.0f;

SceneId LeaderBoard::onEvent(const sf::Event &event) {
    return (sf::Event::KeyPressed == event.type and sf::Keyboard::Escape == event.key.code) ? nullSceneId : Scene::onEvent(event);
}

SceneId LeaderBoard::update(const sf::RenderWindow &window, SceneManager &sceneManager, Assets &assets, sf::Time elapsed) {
    const auto[windowWidth, windowHeight] = window.getSize();
    const auto windowXCenter = windowWidth / 2.0f;
    const auto actionY = windowHeight - BOTTOM_PADDING - mAction.getLocalBounds().height / 2.0f;
    const auto scoreY = (actionY - TOP_PADDING) / 2.0f;

    if (auto &audioManager = assets.getAudioManager(); SoundTrackId::AmbientStarfield != audioManager.getPlaying()) {
        audioManager.play(SoundTrackId::AmbientStarfield);
    }

    mAction.setPosition(windowXCenter, actionY);
    mScore.setPosition(windowXCenter, scoreY);

    return Scene::update(window, sceneManager, assets, elapsed);
}

void LeaderBoard::render(sf::RenderTarget &window) const {
    window.draw(mAction);
    window.draw(mScore);
}

Scene &LeaderBoard::setup(const sf::RenderWindow &window, Assets &assets) {
    mAction.setFont(assets.getFontsManager().getFont(FontId::Mechanical));
    mAction.setString("[ESC]");
    mAction.setCharacterSize(32.0f);
    helpers::centerOrigin(mAction);

    mScore.setFont(assets.getFontsManager().getFont(FontId::Mechanical));
    mScore.setCharacterSize(64.0f);

    pubsub::subscribe<messages::GameOver>(*this);
    return Scene::setup(window, assets);
}

void LeaderBoard::operator()(const messages::GameOver &message) {
    char buffer[64];
    snprintf(buffer, std::size(buffer), "  Game Over\n\n\nScore: %05u", message.score);
    mScore.setString(buffer);
    helpers::centerOrigin(mScore);
}
