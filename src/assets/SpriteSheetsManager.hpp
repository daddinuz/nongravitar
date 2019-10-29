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
#include <vector>
#include <Sprite.hpp>
#include <assets/TexturesManager.hpp>

namespace nongravitar::assets {
    enum class SpriteSheetId {
        Bullet = 0,
        Bunker = 1,
        Planet = 2,
        SpaceShip = 3,
        Supply = 4,
        Terrain = 5,
        Tractor = 6,
    };

    class SpriteSheetsManager final {
        using Frame = sf::IntRect;

    public:
        SpriteSheetsManager() = delete; // no default-constructible

        explicit SpriteSheetsManager(const TexturesManager &texturesManager);

        SpriteSheetsManager(const SpriteSheetsManager &) = delete; // no copy-constructible
        SpriteSheetsManager &operator=(const SpriteSheetsManager &) = delete; // no copy-assignable

        SpriteSheetsManager(SpriteSheetsManager &&) = delete; // no move-constructible
        SpriteSheetsManager &operator=(SpriteSheetsManager &&) = delete; // no move-assignable

        [[nodiscard]] inline Frame getFrame(const SpriteSheetId spriteSheetId, const std::size_t frameId) const {
            return mSpriteSheets.at(helpers::enumValue(spriteSheetId)).at(frameId);
        }

        [[nodiscard]] inline Sprite getSprite(const SpriteSheetId spriteSheetId, const std::size_t frameId) const {
            return {getTexture(spriteSheetId), getFrame(spriteSheetId, frameId)};
        }

        [[nodiscard]] inline const sf::Texture &getTexture(const SpriteSheetId spriteSheetId) const {
            return *mTextures.at(helpers::enumValue(spriteSheetId));
        }

    private:
        void load(const TexturesManager &texturesManager, SpriteSheetId spriteSheetId, TextureId textureId, sf::Vector2u frameSize);

        std::array<std::vector<Frame>, 7> mSpriteSheets;
        std::array<const sf::Texture *, 7> mTextures;
    };
}
