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

#include <assets/SpriteSheetsManager.hpp>

using namespace gravitar;
using namespace gravitar::assets;

void SpriteSheetsManager::initialize(const TexturesManager &textureManager) {
    std::array<const std::tuple<SpriteSheetId, TextureId, sf::Vector2u>, 4> items = {
            std::make_tuple<SpriteSheetId, TextureId, sf::Vector2u>(SpriteSheetId::SpaceShip, TextureId::SpaceShip, {32, 32}),
            std::make_tuple<SpriteSheetId, TextureId, sf::Vector2u>(SpriteSheetId::Bullet, TextureId::Bullet, {8, 8}),
            std::make_tuple<SpriteSheetId, TextureId, sf::Vector2u>(SpriteSheetId::Bunker, TextureId::Bunker, {24, 48}),
            std::make_tuple<SpriteSheetId, TextureId, sf::Vector2u>(SpriteSheetId::Terrain, TextureId::Terrain, {1, 56}),
    };

    for (const auto &i : items) {
        mSpriteSheets.emplace(std::get<0>(i), SpriteSheet::from(textureManager.get(std::get<1>(i)), std::get<2>(i)));
    }
}

const SpriteSheet &SpriteSheetsManager::get(SpriteSheetId id) const noexcept {
    return mSpriteSheets.at(id);
}
