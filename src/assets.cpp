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

#define fontPath(filename) \
    GRAVITAR_DIRECTORY "/assets/fonts/" filename

#define soundtrackPath(filename) \
    GRAVITAR_DIRECTORY "/assets/soundtracks/" filename

static constexpr decltype(auto) TEXTURE_PATH = GRAVITAR_DIRECTORY "/assets/textures";

class AssetsInitializationError final : public std::exception {
public:
    AssetsInitializationError() noexcept = default;

    explicit AssetsInitializationError(const char *msg) noexcept : msg(msg) {};

    [[nodiscard]] const char *what() const noexcept override {
        return msg ? msg : "Unable to initialize asset";
    }

private:
    const char *msg{nullptr};
};

void FontsManager::initialize() {
    if (!mMechanicalFont.loadFromFile(fontPath("mechanical.otf"))) {
        throw AssetsInitializationError(trace("Unable to load file: "
                                                      fontPath("mechanical.otf")));
    }
}

const sf::Font &FontsManager::getMechanicalFont() const noexcept {
    return mMechanicalFont;
}

void SoundtracksManager::initialize() {
    if (mTitleSoundtrack.openFromFile(soundtrackPath("main-theme.wav"))) {
        mTitleSoundtrack.setLoop(true);
    } else {
        throw AssetsInitializationError(trace("Unable to load file: "
                                                      soundtrackPath("main-theme.wav")));
    }
}

void SoundtracksManager::togglePlaying() noexcept {
    if (mCurrentlyPlaying) {
        switch (mCurrentlyPlaying->getStatus()) {
            case sf::Music::Stopped:
            case sf::Music::Paused:mCurrentlyPlaying->play();
                break;

            default: mCurrentlyPlaying->pause();
                break;
        }
    }
}

void SoundtracksManager::playMainTheme() noexcept {
    if (mCurrentlyPlaying) {
        mCurrentlyPlaying->stop();
    }

    mTitleSoundtrack.play();
    mCurrentlyPlaying = &mTitleSoundtrack;
}

void SpriteSheetsManager::initialize() {
    std::array<const std::tuple<const char *, sf::Vector2u, SpriteSheetId>, 1> items = {
            std::make_tuple<const char *, sf::Vector2u, SpriteSheetId>("spaceship.png", {32, 32}, SpriteSheetId::SpaceShip),
    };

    for (decltype(auto) i : items) {
        load(std::get<0>(i), std::get<1>(i), std::get<2>(i));
    }
}

const SpriteSheet &SpriteSheetsManager::getSpriteSheet(SpriteSheetId id) const noexcept {
    return mSpriteSheets.at(id);
}

void SpriteSheetsManager::load(const char *const filename, const sf::Vector2u frameSize, const SpriteSheetId id) {
    char path[256];
    std::snprintf(path, sizeof(path) / sizeof(path[0]), "%s/%s", TEXTURE_PATH, filename);

    if (decltype(auto) texture = mTextures[id]; texture.loadFromFile(path)) {
        texture.setSmooth(true);
        mSpriteSheets.emplace(id, SpriteSheet::from(texture, frameSize.x, frameSize.y));
    } else {
        std::snprintf(path, 256, "%sUnable to load texture: %s", __TRACE__, filename);
        throw AssetsInitializationError(path);
    }
}
