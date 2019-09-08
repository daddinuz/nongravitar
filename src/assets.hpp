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
#include "spritesheet.hpp"

namespace gravitar {
    class FontsManager final {
    public:
        friend class Game;

        FontsManager(const FontsManager &) = delete; // no copy-constructible
        FontsManager &operator=(const FontsManager &) = delete; // no copy-assignable

        FontsManager(FontsManager &&) = delete; // no move-constructible
        FontsManager &operator=(FontsManager &&) = delete; // no move-assignable

        void initialize();

        const sf::Font &getMechanicalFont() const noexcept;

    private:
        FontsManager() = default; // private default constructor, only Game can construct this class

        sf::Font mMechanicalFont;
    };

    class SoundtracksManager final {
    public:
        friend class Game;

        SoundtracksManager(const SoundtracksManager &) = delete; // no copy-constructible
        SoundtracksManager &operator=(const SoundtracksManager &) = delete; // no copy-assignable

        SoundtracksManager(SoundtracksManager &&) = delete; // no move-constructible
        SoundtracksManager &operator=(SoundtracksManager &&) = delete; // no move-assignable

        void initialize();

        void togglePlaying() noexcept;

        void playMainTheme() noexcept;

    private:
        SoundtracksManager() = default; // private default constructor, only Game can construct this class

        sf::Music mTitleSoundtrack;
        sf::Music *mCurrentlyPlaying{nullptr};
    };

    enum class SpriteSheetId {
        SpaceShip,
    };

    class SpriteSheetsManager final {
    public:
        friend class Game;

        SpriteSheetsManager(const SpriteSheetsManager &) = delete; // no copy-constructible
        SpriteSheetsManager &operator=(const SpriteSheetsManager &) = delete; // no copy-assignable

        SpriteSheetsManager(SpriteSheetsManager &&) = delete; // no move-constructible
        SpriteSheetsManager &operator=(SpriteSheetsManager &&) = delete; // no move-assignable

        void initialize();

        [[nodiscard]] const SpriteSheet &getSpriteSheet(SpriteSheetId id) const noexcept;

    private:
        SpriteSheetsManager() = default; // private default constructor, only Game can construct this class

        void load(const char *filename, sf::Vector2u frameSize, SpriteSheetId id);

        std::map<SpriteSheetId, sf::Texture> mTextures;
        std::map<SpriteSheetId, SpriteSheet> mSpriteSheets;
    };
}
