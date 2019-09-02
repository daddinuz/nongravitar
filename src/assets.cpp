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

#define fontPath(filename) \
    GRAVITAR_DIRECTORY "/assets/fonts/" filename

#define soundtrackPath(filename) \
    GRAVITAR_DIRECTORY "/assets/soundtracks/" filename

#define texturePath(filename) \
    GRAVITAR_DIRECTORY "/assets/textures/" filename

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

void gravitar::FontsManager::initialize() {
    if (!mMechanicalFont.loadFromFile(fontPath("mechanical.otf"))) {
        throw AssetsInitializationError(trace("Unable to load file: "
                                                      fontPath("mechanical.otf")));
    }
}

const sf::Font &gravitar::FontsManager::getMechanicalFont() const noexcept {
    return mMechanicalFont;
}

void gravitar::SoundtracksManager::initialize() {
    if (mTitleSoundtrack.openFromFile(soundtrackPath("main-theme.wav"))) {
        mTitleSoundtrack.setLoop(true);
    } else {
        throw AssetsInitializationError(trace("Unable to load file: "
                                                      soundtrackPath("main-theme.wav")));
    }
}

void gravitar::SoundtracksManager::togglePlaying() noexcept {
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

void gravitar::SoundtracksManager::playMainTheme() noexcept {
    if (mCurrentlyPlaying) {
        mCurrentlyPlaying->stop();
    }

    mTitleSoundtrack.play();
    mCurrentlyPlaying = &mTitleSoundtrack;
}

void gravitar::TextureManager::initialize() {
    if (!mAirCraftTexture.loadFromFile(texturePath("aircraft.png"))) {
        throw AssetsInitializationError(trace("Unable to load file: "
                                                      texturePath("aircraft.png")));
    }

    if (!mTestTexture.loadFromFile(texturePath("test.png"))) {
        throw AssetsInitializationError(trace("Unable to load file: "
                                                      texturePath("test.png")));
    }
}

const sf::Texture &gravitar::TextureManager::getAirCraftTexture() const {
    return mAirCraftTexture;
}

const sf::Texture &gravitar::TextureManager::getTestTexture() const {
    return mTestTexture;
}
