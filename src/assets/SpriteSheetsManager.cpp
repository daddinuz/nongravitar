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

using namespace nongravitar::assets;

SpriteSheetsManager::SpriteSheetsManager(const TexturesManager &texturesManager) {
    // WARNING: the order of the elements in the vector must match the underlying value of SpriteSheetId enumerators!
    mSpriteSheets.emplace_back(texturesManager.getTexture(TextureId::Bullet), sf::Vector2u(8, 8));
    mSpriteSheets.emplace_back(texturesManager.getTexture(TextureId::Bunker), sf::Vector2u(56, 56));
    mSpriteSheets.emplace_back(texturesManager.getTexture(TextureId::SpaceShip), sf::Vector2u(32, 32));
    mSpriteSheets.emplace_back(texturesManager.getTexture(TextureId::Supply), sf::Vector2u(22, 22));
    mSpriteSheets.emplace_back(texturesManager.getTexture(TextureId::Terrain), sf::Vector2u(14, 1));
}
