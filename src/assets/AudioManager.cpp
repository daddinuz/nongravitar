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
#include <helpers.hpp>
#include <assets/AudioManager.hpp>

using namespace nongravitar::assets;

AudioManager::AudioManager() {
    // sounds
    load("hit.ogg", SoundId::Hit);
    load("shot.ogg", SoundId::Shot);
    load("tractor.ogg", SoundId::Tractor);
    load("explosion.ogg", SoundId::Explosion);

    // soundtracks
    load("Drozerix-AmbientStarfield.flac", SoundTrackId::AmbientStarfield);
    load("Drozerix-ComputerAdventures.flac", SoundTrackId::ComputerAdventures);
    load("Drozerix-ComputerF__k.flac", SoundTrackId::ComputerF__k);
}

void AudioManager::play(const SoundId id) noexcept {
    if (not mMuted) {
        mSounds.at(helpers::enumValue(id)).play();
    }
}

void AudioManager::play(const SoundTrackId id) noexcept {
    if (not mMuted) {
        if (SoundTrackId::None != mCurrentSoundtrackId) {
            mSoundtracks.at(helpers::enumValue(mCurrentSoundtrackId)).stop();
        }

        mCurrentSoundtrackId = id;

        if (SoundTrackId::None != mCurrentSoundtrackId) {
            mSoundtracks.at(helpers::enumValue(mCurrentSoundtrackId)).play();
        }
    }
}

void AudioManager::toggle() noexcept {
    mMuted ^= true;

    if (SoundTrackId::None != mCurrentSoundtrackId) {
        auto &soundtrack = mSoundtracks.at(helpers::enumValue(mCurrentSoundtrackId));
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

void AudioManager::load(const char *const filename, const SoundId id) {
    auto path = std::string(NONGRAVITAR_SOUNDS_PATH "/") + filename;

    if (auto &soundBuffer = mSoundBuffers[helpers::enumValue(id)]; soundBuffer.loadFromFile(path)) {
        mSounds[helpers::enumValue(id)].setBuffer(soundBuffer);
    } else {
        path.insert(0, __TRACE__ "Unable to load sound: ");
        throw std::runtime_error(path);
    }
}

void AudioManager::load(const char *const filename, const SoundTrackId id) {
    auto path = std::string(NONGRAVITAR_SOUNDTRACKS_PATH "/") + filename;

    if (auto &soundtrack = mSoundtracks[helpers::enumValue(id)]; soundtrack.openFromFile(path)) {
        soundtrack.setLoop(true);
    } else {
        path.insert(0, __TRACE__ "Unable to load soundtrack: ");
        throw std::runtime_error(path);
    }
}
