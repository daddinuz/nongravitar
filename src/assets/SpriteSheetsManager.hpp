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
#include <SpriteSheet.hpp>
#include <assets/TexturesManager.hpp>

namespace gravitar::assets {
    enum class SpriteSheetId {
        SpaceShip,
        Bullet,
        Bunker,
    };

    class SpriteSheetsManager final {
    public:
        SpriteSheetsManager() = default; // default-constructible

        SpriteSheetsManager(const SpriteSheetsManager &) = delete; // no copy-constructible
        SpriteSheetsManager &operator=(const SpriteSheetsManager &) = delete; // no copy-assignable

        SpriteSheetsManager(SpriteSheetsManager &&) = delete; // no move-constructible
        SpriteSheetsManager &operator=(SpriteSheetsManager &&) = delete; // no move-assignable

        [[nodiscard]] const SpriteSheet &get(SpriteSheetId id) const noexcept;

        /**
         * Initialize assets loading them into memory.
         *
         * @warning
         *  This method should be called exactly once in the life-cycle of this object, any usage of this object
         *  without proper initialization will result in a error.
         *
         *  Calling this method with a `TextureManager` not properly initialized will result in an error.
         */
        void initialize(const TexturesManager &textureManager);

    private:
        std::map<SpriteSheetId, SpriteSheet> mSpriteSheets;
    };
}
