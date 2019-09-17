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
#include <scene/TitleScreen.hpp>

using namespace gravitar::assets;
using namespace gravitar::scene;

TitleScreen::TitleScreen(const SceneId nextSceneId, const FontManager &fontManager, const TextureManager &textureManager) :
        mGravitarTitle(textureManager.get(assets::TextureId::GravitarTitle)),
        mSpaceLabel("[SPACE]", fontManager.get(FontId::Mechanical), 24),
        mNextSceneId{nextSceneId} {
    helpers::centerOrigin(mGravitarTitle, mGravitarTitle.getLocalBounds());
    helpers::centerOrigin(mSpaceLabel, mSpaceLabel.getLocalBounds());
}

void TitleScreen::adjustAudio(gravitar::assets::AudioManager &audioManager) noexcept {
    if (assets::SoundTrackId::MainTheme != audioManager.getPlaying()) {
        audioManager.play(SoundTrackId::MainTheme);
    }
}

SceneId TitleScreen::onEvent(const sf::Event &event) noexcept {
    return (sf::Event::KeyPressed == event.type and sf::Keyboard::Space == event.key.code) ? mNextSceneId : getId();
}

void TitleScreen::update(const sf::RenderWindow &window, sf::Time elapsed) noexcept {
    (void) elapsed;

    const auto[windowWidth, windowHeight] = window.getSize();
    mGravitarTitle.setPosition(windowWidth / 2.0f, windowHeight / 3.14f);
    mSpaceLabel.setPosition(windowWidth / 2.0f, windowHeight / 1.12f);
}

void TitleScreen::render(sf::RenderTarget &window) const noexcept {
    window.draw(mGravitarTitle);
    window.draw(mSpaceLabel);
}
