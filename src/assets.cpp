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

#define fonts_path(filename) \
    GRAVITAR_DIRECTORY "/assets/fonts/" filename

#define soundtrack_path(filename) \
    GRAVITAR_DIRECTORY "/assets/soundtracks/" filename

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
    if (!mMechanicalFont.loadFromFile(fonts_path("mechanical.otf"))) {
        throw AssetsInitializationError("Unable to load file: " fonts_path("mechanical.otf"));
    }
}

const sf::Font &gravitar::FontsManager::mechanicalFont() const noexcept {
    return mMechanicalFont;
}

void gravitar::SoundtracksManager::initialize() {
    if (mTitleSoundtrack.openFromFile(soundtrack_path("opening.wav"))) {
        mTitleSoundtrack.setLoop(true);
    } else {
        throw AssetsInitializationError("Unable to load file: " soundtrack_path("opening.wav"));
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

void gravitar::SoundtracksManager::playTitleSoundtrack() noexcept {
    if (mCurrentlyPlaying) {
        mCurrentlyPlaying->stop();
    }

    mTitleSoundtrack.play();
    mCurrentlyPlaying = &mTitleSoundtrack;
}

void gravitar::AssetsManager::initialize() {
    mFontsManager.initialize();
    mSoundtracksManager.initialize();
}

const gravitar::FontsManager &gravitar::AssetsManager::fonts() const noexcept {
    return mFontsManager;
}

gravitar::SoundtracksManager &gravitar::AssetsManager::soundtracks() noexcept {
    return mSoundtracksManager;
}
