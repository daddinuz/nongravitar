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

#include "assets.hpp"
#include "trace.hpp"

using namespace gravitar;

static constexpr decltype(auto) FONTS_PATH = GRAVITAR_DIRECTORY "/assets/fonts";
static constexpr decltype(auto) TEXTURES_PATH = GRAVITAR_DIRECTORY "/assets/textures";
static constexpr decltype(auto) SOUNDTRACKS_PATH = GRAVITAR_DIRECTORY "/assets/soundtracks";

/*
 * AssetsInitializationError
 */

class AssetsInitializationError final : public std::exception {
public:
    explicit AssetsInitializationError(const char *msg) noexcept : msg(msg) {};

    [[nodiscard]] const char *what() const noexcept override {
        return msg ? msg : "Unable to initialize asset";
    }

private:
    const char *msg{nullptr};
};

/*
 * SoundtracksManager
 */

void SoundTracksManager::togglePlaying() noexcept {
    if (mCurrentlyPlaying) {
        switch (mCurrentlyPlaying->getStatus()) {
            case sf::Music::Playing: mCurrentlyPlaying->pause();
                break;

            default: mCurrentlyPlaying->play();
                break;
        }
    }
}

void SoundTracksManager::play(const SoundTrackId id) noexcept {
    if (mCurrentlyPlaying) {
        mCurrentlyPlaying->stop();
    }

    decltype(auto) soundtrack = mSoundtracks.at(id);
    mCurrentlyPlaying = &soundtrack;
    soundtrack.play();
}

void SoundTracksManager::initialize() {
    std::array<const std::tuple<const char *, SoundTrackId>, 1> items = {
            std::make_tuple<const char *, SoundTrackId>("main-theme.wav", SoundTrackId::MainTheme),
    };

    for (decltype(auto) i : items) {
        load(std::get<0>(i), std::get<1>(i));
    }
}

void SoundTracksManager::load(const char *const filename, const SoundTrackId id) {
    char path[256];
    std::snprintf(path, std::size(path), "%s/%s", SOUNDTRACKS_PATH, filename);

    if (decltype(auto) soundtrack = mSoundtracks[id]; soundtrack.openFromFile(path)) {
        soundtrack.setLoop(true);
    } else {
        std::snprintf(path, std::size(path), "%sUnable to load soundtrack: %s", __TRACE__, filename);
        throw AssetsInitializationError(path);
    }
}

/*
 * FontsManager
 */

const sf::Font &FontsManager::get(const FontId id) const noexcept {
    return mFonts.at(id);
}

void FontsManager::initialize() {
    std::array<const std::tuple<const char *, FontId>, 1> items = {
            std::make_tuple<const char *, FontId>("mechanical.otf", FontId::Mechanical),
    };

    for (decltype(auto) i : items) {
        load(std::get<0>(i), std::get<1>(i));
    }
}

void FontsManager::load(const char *const filename, const FontId id) {
    char path[256];
    std::snprintf(path, std::size(path), "%s/%s", FONTS_PATH, filename);

    if (decltype(auto) soundtrack = mFonts[id]; !soundtrack.loadFromFile(path)) {
        std::snprintf(path, std::size(path), "%sUnable to load font: %s", __TRACE__, filename);
        throw AssetsInitializationError(path);
    }
}

/*
 * TexturesManager
 */

const sf::Texture &TexturesManager::get(TextureId id) const noexcept {
    return mTextures.at(id);
}

void TexturesManager::initialize() {
    std::array<const std::tuple<const char *, TextureId>, 3> items = {
            std::make_tuple<const char *, TextureId>("gravitar-title.png", TextureId::GravitarTitle),
            std::make_tuple<const char *, TextureId>("spaceship.png", TextureId::SpaceShip),
            std::make_tuple<const char *, TextureId>("bullet.png", TextureId::Bullet),
    };

    for (decltype(auto) i : items) {
        load(std::get<0>(i), std::get<1>(i));
    }
}

void TexturesManager::load(const char *filename, TextureId id) {
    char path[256];
    std::snprintf(path, std::size(path), "%s/%s", TEXTURES_PATH, filename);

    if (decltype(auto) texture = mTextures[id]; texture.loadFromFile(path)) {
        texture.setSmooth(true);
    } else {
        std::snprintf(path, std::size(path), "%sUnable to load texture: %s", __TRACE__, filename);
        throw AssetsInitializationError(path);
    }
}

/*
 * GravitarTitle
 */

void GravitarTitle::setPosition(float x, float y) {
    mSprite.setPosition(x, y);
}

void GravitarTitle::initialize(const TexturesManager &texturesManager) {
    mSprite.setTexture(texturesManager.get(TextureId::GravitarTitle), true);

    decltype(auto) bounds = mSprite.getLocalBounds();
    mSprite.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
}

void GravitarTitle::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    (void) states;
    target.draw(mSprite);
}

/*
 * SpaceLabel
 */

static constexpr std::array<sf::Uint8, 8> WHITE_TRANSITION = {
        255,
        240,
        220,
        200,
        180,
        200,
        220,
        240,
};

void SpaceLabel::update(const sf::Time &time) {
    mInstance.update(time);
}

void SpaceLabel::setPosition(float x, float y) {
    mInstance->setPosition(x, y);
}

SpaceLabel::SpaceLabel() :
        mInstance(8, [](auto &drawable, auto &frames) {
                      sf::Color fillColor = drawable.getFillColor();
                      fillColor.a = *++frames;
                      drawable.setFillColor(fillColor);
                  },
                  sf::Text(),
                  Cycle<const Transition, Transition::const_iterator>(WHITE_TRANSITION.cbegin(), WHITE_TRANSITION.cend())) {}

void SpaceLabel::initialize(const FontsManager &fontsManager) {
    mInstance->setFont(fontsManager.get(FontId::Mechanical));
    mInstance->setCharacterSize(24);
    mInstance->setString("[SPACE]");

    decltype(auto) bounds = mInstance->getLocalBounds();
    mInstance->setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
}

void SpaceLabel::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    (void) states;
    target.draw(mInstance);
}

/*
 * SpriteSheetsManager
 */

const SpriteSheet &SpriteSheetsManager::get(SpriteSheetId id) const noexcept {
    return mSpriteSheets.at(id);
}

void SpriteSheetsManager::initialize(const TexturesManager &texturesManager) {
    std::array<const std::tuple<SpriteSheetId, TextureId, sf::Vector2u>, 2> items = {
            std::make_tuple<SpriteSheetId, TextureId, sf::Vector2u>(SpriteSheetId::SpaceShip, TextureId::SpaceShip, {32, 32}),
            std::make_tuple<SpriteSheetId, TextureId, sf::Vector2u>(SpriteSheetId::Bullet, TextureId::Bullet, {8, 8}),
    };

    for (decltype(auto) i : items) {
        mSpriteSheets.emplace(std::get<0>(i), SpriteSheet::from(texturesManager.get(std::get<1>(i)), std::get<2>(i)));
    }
}
