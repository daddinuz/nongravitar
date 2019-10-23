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

#pragma once

#include <array>
#include <SFML/Audio.hpp>

namespace nongravitar::assets {
    enum class SoundTrackId : std::size_t {
        AmbientStarfield = 0,
        ComputerAdventures,
        ComputerF__k,
        None = std::numeric_limits<std::size_t>::max(),
    };

    enum class SoundId : std::size_t {
        Hit = 0,
        Shot,
        Tractor,
        Explosion,
    };

    class AudioManager final {
    public:
        AudioManager();

        AudioManager(const AudioManager &) = delete; // no copy-constructible
        AudioManager &operator=(const AudioManager &) = delete; // no copy-assignable

        AudioManager(AudioManager &&) = delete; // no move-constructible
        AudioManager &operator=(AudioManager &&) = delete; // no move-assignable

        void play(SoundId id) noexcept;

        void play(SoundTrackId id) noexcept;

        void toggle() noexcept;

        [[nodiscard]] inline SoundTrackId getPlaying() const noexcept {
            return mCurrentSoundtrackId;
        }

    private:
        void load(const char *filename, SoundId id);
        void load(const char *filename, SoundTrackId id);

        std::array<sf::Music, 3> mSoundtracks;
        std::array<sf::SoundBuffer, 4> mSoundBuffers;
        std::array<sf::Sound, 4> mSounds;
        SoundTrackId mCurrentSoundtrackId{SoundTrackId::None};
        bool mMuted{false};
    };
}
