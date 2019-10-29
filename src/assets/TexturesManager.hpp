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
#include <helpers.hpp>
#include <SFML/Graphics.hpp>

namespace nongravitar::assets {
    enum class TextureId : std::size_t {
        Bullet = 0,
        Bunker = 1,
        Planet = 2,
        SpaceShip = 3,
        Supply = 4,
        Terrain = 5,
        Title = 6,
        Tractor = 7,
    };

    class TexturesManager final {
    public:
        TexturesManager();

        TexturesManager(const TexturesManager &) = delete; // no copy-constructible
        TexturesManager &operator=(const TexturesManager &) = delete; // no copy-assignable

        TexturesManager(TexturesManager &&) = delete; // no move-constructible
        TexturesManager &operator=(TexturesManager &&) = delete; // no move-assignable

        [[nodiscard]] inline const sf::Texture &getTexture(const TextureId textureId) const {
            return mTextures.at(helpers::enumValue(textureId));
        }

    private:
        void load(const char *filename, TextureId textureId);

        std::array<sf::Texture, 8> mTextures;
    };
}
