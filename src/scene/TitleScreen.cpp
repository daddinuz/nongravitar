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

using namespace nongravitar;
using namespace nongravitar::scene;
using namespace nongravitar::assets;

constexpr auto TOP_PADDING = 64.0f;
constexpr auto MIDDLE_PADDING = 96.0f;
constexpr auto BOTTOM_PADDING = 32.0f;

TitleScreen::TitleScreen(const SceneId solarSystemSceneId, Assets &assets) :
        mTitle(assets.getTexturesManager().get(TextureId::Title)),
        mSpaceLabel("[SPACE]", assets.getFontsManager().get(FontId::Mechanical), 32.0f),
        mSolarSystemSceneId{solarSystemSceneId} {
    helpers::centerOrigin(mTitle, mTitle.getLocalBounds());
    helpers::centerOrigin(mSpaceLabel, mSpaceLabel.getLocalBounds());
}

SceneId TitleScreen::onEvent(const sf::Event &event) noexcept {
    return (sf::Event::KeyPressed == event.type and sf::Keyboard::Space == event.key.code) ? mSolarSystemSceneId : getSceneId();
}

SceneId TitleScreen::update(const sf::RenderWindow &window, SceneManager &sceneManager, Assets &assets, sf::Time elapsed) noexcept {
    const auto[windowWidth, windowHeight] = window.getSize();
    const auto spaceLabelHeight = mSpaceLabel.getLocalBounds().height;
    const auto scaleFactor = (windowHeight - TOP_PADDING - MIDDLE_PADDING - spaceLabelHeight - BOTTOM_PADDING) / mTitle.getLocalBounds().height;

    if (auto &audioManager = assets.getAudioManager(); SoundTrackId::AmbientStarfield != audioManager.getPlaying()) {
        audioManager.play(SoundTrackId::AmbientStarfield);
    }

    mTitle.setScale(scaleFactor, scaleFactor);
    mTitle.setPosition(windowWidth / 2.0f, TOP_PADDING + mTitle.getGlobalBounds().height / 2.0f);
    mSpaceLabel.setPosition(windowWidth / 2.0f, TOP_PADDING + mTitle.getGlobalBounds().height + MIDDLE_PADDING + spaceLabelHeight / 2.0f);

    return Scene::update(window, sceneManager, assets, elapsed);
}

void TitleScreen::render(sf::RenderTarget &window) const noexcept {
    window.draw(mTitle);
    window.draw(mSpaceLabel);
}
