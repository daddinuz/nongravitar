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

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

namespace gravitar {
    class FontsManager final {
    public:
        friend class AssetsManager;

        FontsManager(const FontsManager &) = delete; // no copy-constructible

        FontsManager &operator=(const FontsManager &) = delete; // no copy-assignable

        void initialize();

        const sf::Font &mechanicalFont() const noexcept;

    private:
        FontsManager() = default; // private default constructor, only AssetsManager can construct this class

        sf::Font mMechanicalFont;
    };

    class SoundtracksManager final {
    public:
        friend class AssetsManager;

        SoundtracksManager(const SoundtracksManager &) = delete; // no copy-constructible

        SoundtracksManager &operator=(const SoundtracksManager &) = delete; // no copy-assignable

        void initialize();

        void togglePlaying() noexcept;

        void playTitleSoundtrack() noexcept;

    private:
        SoundtracksManager() = default; // private default constructor, only AssetsManager can construct this class

        sf::Music mTitleSoundtrack;
        sf::Music *mCurrentlyPlaying{nullptr};
    };

    class AssetsManager final {
    public:
        friend class Game;

        AssetsManager(const AssetsManager &) = delete; // no copy-constructible

        AssetsManager &operator=(const AssetsManager &) = delete; // no copy-assignable

        void initialize();

        [[nodiscard]] const FontsManager &fonts() const noexcept;

        [[nodiscard]] SoundtracksManager &soundtracks() noexcept;

    private:
        AssetsManager() = default; // private default constructor, only Game can construct this class

        FontsManager mFontsManager;
        SoundtracksManager mSoundtracksManager;
    };
}
