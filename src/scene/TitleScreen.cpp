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
constexpr auto BOTTOM_PADDING = 64.0f;

TitleScreen::TitleScreen(const SceneId solarSystemSceneId) : mSolarSystemSceneId{solarSystemSceneId} {}

SceneId TitleScreen::onEvent(const sf::Event &event) {
    return (sf::Event::KeyPressed == event.type and sf::Keyboard::Space == event.key.code) ? mSolarSystemSceneId : Scene::onEvent(event);
}

SceneId TitleScreen::update(const sf::RenderWindow &window, SceneManager &sceneManager, Assets &assets, sf::Time elapsed) {
    const auto[windowWidth, windowHeight] = window.getSize();
    const auto windowXCenter = windowWidth / 2.0f;
    const auto actionHalfHeight = mAction.getLocalBounds().height / 2.0f;
    const auto scaleFactor = (windowHeight - TOP_PADDING - MIDDLE_PADDING - actionHalfHeight - BOTTOM_PADDING) / mTitle.getLocalBounds().height;

    if (auto &audioManager = assets.getAudioManager(); SoundTrackId::AmbientStarfield != audioManager.getPlaying()) {
        audioManager.play(SoundTrackId::AmbientStarfield);
    }

    mAction.setPosition(windowXCenter, windowHeight - BOTTOM_PADDING - actionHalfHeight);

    mTitle.setScale(scaleFactor, scaleFactor);
    mTitle.setPosition(windowXCenter, TOP_PADDING + mTitle.getGlobalBounds().height / 2.0f);

    return Scene::update(window, sceneManager, assets, elapsed);
}

void TitleScreen::render(sf::RenderTarget &window) const {
    window.draw(mTitle);
    window.draw(mAction);
}

Scene &TitleScreen::setup(const sf::RenderWindow &window, Assets &assets) {
    mAction.setFont(assets.getFontsManager().getFont(FontId::Mechanical));
    mAction.setString("[SPACE]");
    mAction.setCharacterSize(32.0f);
    helpers::centerOrigin(mAction);

    mTitle.setTexture(assets.getTexturesManager().getTexture(TextureId::Title));
    helpers::centerOrigin(mTitle);

    return Scene::setup(window, assets);
}
