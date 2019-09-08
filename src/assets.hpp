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

#include <map>
#include <memory>
#include <typeindex>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "animation.hpp"
#include "cycle.hpp"
#include "spritesheet.hpp"
#include "wrappers.hpp"

namespace gravitar {
    class FontsManager;

    namespace AnimationId {
        struct Continue final {
            Continue() = delete; // no default-constructible
            Continue(const Continue &) = delete; // no copy-constructible
            Continue(Continue &&) = delete; // no move-constructible

            using Drawable = sf::Text;
            using Frame = Cycle<const std::array<sf::Uint8, 8>>;
            using Type = Animation<Drawable, Frame>;
        };
    }

    class AnimationsManager {
    public:
        friend class Game;

        AnimationsManager(const AnimationsManager &) = delete; // no copy-constructible
        AnimationsManager &operator=(const AnimationsManager &) = delete; // no copy-assignable

        AnimationsManager(AnimationsManager &&) = delete; // no move-constructible
        AnimationsManager &operator=(AnimationsManager &&) = delete; // no move-assignable

        void initialize(const FontsManager &fontsManager);

        template<typename A>
        typename A::Type &get() noexcept;

    private:
        AnimationsManager() = default; // private default constructor, only Game can construct this class

        template<typename T>
        inline constexpr decltype(auto) getTypeIndex() noexcept {
            return std::type_index(typeid(T));
        }

        std::map<std::type_index, std::unique_ptr<sf::Drawable>> mAnimations;
        const FontsManager *mFontsManager{nullptr};
    };

    enum class FontId {
        Mechanical
    };

    class FontsManager final {
    public:
        friend class Game;

        FontsManager(const FontsManager &) = delete; // no copy-constructible
        FontsManager &operator=(const FontsManager &) = delete; // no copy-assignable

        FontsManager(FontsManager &&) = delete; // no move-constructible
        FontsManager &operator=(FontsManager &&) = delete; // no move-assignable

        void initialize();

        [[nodiscard]] const sf::Font &get(FontId id) const noexcept;

    private:
        FontsManager() = default; // private default constructor, only Game can construct this class

        void load(const char *filename, FontId id);

        std::map<FontId, sf::Font> mFonts;
    };

    enum class SoundTrackId {
        MainTheme,
    };

    class SoundTracksManager final {
    public:
        friend class Game;

        SoundTracksManager(const SoundTracksManager &) = delete; // no copy-constructible
        SoundTracksManager &operator=(const SoundTracksManager &) = delete; // no copy-assignable

        SoundTracksManager(SoundTracksManager &&) = delete; // no move-constructible
        SoundTracksManager &operator=(SoundTracksManager &&) = delete; // no move-assignable

        void initialize();

        void togglePlaying() noexcept;

        void play(SoundTrackId id) noexcept;

    private:
        SoundTracksManager() = default; // private default constructor, only Game can construct this class

        void load(const char *filename, SoundTrackId id);

        std::map<SoundTrackId, sf::Music> mSoundtracks;
        sf::Music *mCurrentlyPlaying{nullptr};
    };

    enum class SpriteId {
        GravitarTitle,
    };

    class SpritesManager final {
    public:
        friend class Game;

        SpritesManager(const SpritesManager &) = delete; // no copy-constructible
        SpritesManager &operator=(const SpritesManager &) = delete; // no copy-assignable

        SpritesManager(SpritesManager &&) = delete; // no move-constructible
        SpritesManager &operator=(SpritesManager &&) = delete; // no move-assignable

        void initialize();

        [[nodiscard]] TransformableDrawable<sf::Sprite> &get(SpriteId id) noexcept;

    private:
        SpritesManager() = default; // private default constructor, only Game can construct this class

        void load(const char *filename, SpriteId id);

        std::map<SpriteId, sf::Texture> mTextures;
        std::map<SpriteId, TransformableDrawable<sf::Sprite>> mSprites;
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

        [[nodiscard]] const SpriteSheet &get(SpriteSheetId id) const noexcept;

    private:
        SpriteSheetsManager() = default; // private default constructor, only Game can construct this class

        void load(const char *filename, sf::Vector2u frameSize, SpriteSheetId id);

        std::map<SpriteSheetId, sf::Texture> mTextures;
        std::map<SpriteSheetId, SpriteSheet> mSpriteSheets;
    };

    /*
     * Implementation
     */

    template<typename A>
    typename A::Type &AnimationsManager::get() noexcept {
        return dynamic_cast<typename A::Type &>(*mAnimations.at(getTypeIndex<A>()));
    }
}
