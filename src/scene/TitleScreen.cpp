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

using namespace gravitar::scene;

TitleScreen::TitleScreen(const SceneId solarSystemSceneId, const assets::TextureManager &textureManager) :
        mGravitarTitle(textureManager.get(assets::TextureId::GravitarTitle)),
        mSolarSystemSceneId{solarSystemSceneId} {
    helpers::centerOrigin(mGravitarTitle, mGravitarTitle.getLocalBounds());
}

void TitleScreen::adjustAudio(gravitar::assets::AudioManager &audioManager) {
    if (assets::SoundTrackId::MainTheme != audioManager.getPlaying()) {
        audioManager.play(assets::SoundTrackId::MainTheme);
    }
}

SceneId TitleScreen::update(const sf::RenderTarget &renderTarget, const sf::Clock &clock) {
    (void) clock;

    const auto[windowWidth, windowHeight] = renderTarget.getSize();
    mGravitarTitle.setPosition(windowWidth / 2.0f, windowHeight / 3.14f);

    return sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ? mSolarSystemSceneId : getId();
}

void TitleScreen::render(sf::RenderTarget &renderTarget) {
    renderTarget.draw(mGravitarTitle);
}
