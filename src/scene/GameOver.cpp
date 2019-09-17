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
#include <scene/GameOver.hpp>

using namespace gravitar::scene;
using namespace gravitar::assets;

GameOver::GameOver(const FontManager &fontManager) :
        mGameOverTitle("Game Over", fontManager.get(FontId::Mechanical), 64),
        mSpaceLabel("[SPACE]", fontManager.get(FontId::Mechanical), 24) {
    helpers::centerOrigin(mGameOverTitle, mGameOverTitle.getLocalBounds());
    helpers::centerOrigin(mSpaceLabel, mSpaceLabel.getLocalBounds());
}

void GameOver::adjustAudio(AudioManager &audioManager) noexcept {
    (void) audioManager;
    // TODO: play game over soundtrack
}

SceneId GameOver::handleEvent(const sf::Event &event) noexcept {
    return (sf::Event::KeyPressed == event.type and sf::Keyboard::Space == event.key.code) ? NullScene : getId();
}

void GameOver::update(const sf::RenderWindow &window, sf::Time elapsed) noexcept {
    (void) elapsed;

    const auto[windowWidth, windowHeight] = window.getSize();
    mGameOverTitle.setPosition(windowWidth / 2.0f, windowHeight / 3.14f);
    mSpaceLabel.setPosition(windowWidth / 2.0f, windowHeight / 1.12f);
}

void GameOver::render(sf::RenderTarget &window) const noexcept {
    window.draw(mGameOverTitle);
    window.draw(mSpaceLabel);
}
