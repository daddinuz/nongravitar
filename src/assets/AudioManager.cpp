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

#include <trace.hpp>
#include <assets/AudioManager.hpp>

using namespace gravitar::assets;

constexpr auto SOUNDS_PATH = GRAVITAR_DIRECTORY "/assets/sounds";
constexpr auto SOUNDTRACKS_PATH = GRAVITAR_DIRECTORY "/assets/soundtracks";

void AudioManager::initialize() {
    std::array<const std::tuple<const char *, SoundId>, 1> sounds = {
            std::make_tuple<const char *, SoundId>("bullet-shot.ogg", SoundId::BulletShot),
    };
    std::array<const std::tuple<const char *, SoundTrackId>, 3> soundtracks = {
            std::make_tuple<const char *, SoundTrackId>("Drozerix-AmbientStarfield.flac", SoundTrackId::AmbientStarfield),
            std::make_tuple<const char *, SoundTrackId>("Drozerix-ComputerAdventures.flac", SoundTrackId::ComputerAdventures),
            std::make_tuple<const char *, SoundTrackId>("Drozerix-ComputerF__k.flac", SoundTrackId::ComputerF__k),
    };

    for (const auto &i : sounds) {
        load(std::get<0>(i), std::get<1>(i));
    }

    for (const auto &i : soundtracks) {
        load(std::get<0>(i), std::get<1>(i));
    }
}

void AudioManager::play(const SoundId id) noexcept {
    if (not mMuted) {
        mSounds.at(id).play();
    }
}

void AudioManager::play(const SoundTrackId id) noexcept {
    if (not mMuted) {
        if (SoundTrackId::None != mCurrentSoundtrackId) {
            mSoundtracks.at(mCurrentSoundtrackId).stop();
        }

        mCurrentSoundtrackId = id;

        if (SoundTrackId::None != mCurrentSoundtrackId) {
            mSoundtracks.at(mCurrentSoundtrackId).play();
        }
    }
}

void AudioManager::toggle() noexcept {
    mMuted ^= true;

    if (SoundTrackId::None != mCurrentSoundtrackId) {
        auto &soundtrack = mSoundtracks.at(mCurrentSoundtrackId);
        switch (soundtrack.getStatus()) {
            case sf::Music::Playing:
                soundtrack.pause();
                break;

            default:
                soundtrack.play();
                break;
        }
    }
}

SoundTrackId AudioManager::getPlaying() const noexcept {
    return mCurrentSoundtrackId;
}

void AudioManager::load(const char *filename, SoundId id) {
    char path[256];
    std::snprintf(path, std::size(path), "%s/%s", SOUNDS_PATH, filename);

    if (decltype(auto) sound = mSoundBuffers[id]; not sound.loadFromFile(path)) {
        std::snprintf(path, std::size(path), "%sUnable to load sound: %s", __TRACE__, filename);
        throw std::runtime_error(path);
    }

    mSounds.emplace(id, mSoundBuffers[id]);
}

void AudioManager::load(const char *const filename, const SoundTrackId id) {
    char path[256];
    std::snprintf(path, std::size(path), "%s/%s", SOUNDTRACKS_PATH, filename);

    if (decltype(auto) soundtrack = mSoundtracks[id]; soundtrack.openFromFile(path)) {
        soundtrack.setLoop(true);
    } else {
        std::snprintf(path, std::size(path), "%sUnable to load soundtrack: %s", __TRACE__, filename);
        throw std::runtime_error(path);
    }
}
