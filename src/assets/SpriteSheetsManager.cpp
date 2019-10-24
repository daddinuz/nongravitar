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
#include <assets/SpriteSheetsManager.hpp>

using namespace nongravitar;
using namespace nongravitar::assets;

Sprite::Sprite(const sf::Texture &texture, Frame frame) : mFrame(frame), mTexture(&texture) {}

void assets::bind(Canvas &canvas, sf::RenderStates &renderStates, const sf::Transformable &transformable, const Sprite &sprite) {
    const auto frame = sprite.getFrame();

    canvas[0].position = {0, 0};
    canvas[1].position = {frame.width, 0};
    canvas[2].position = {frame.width, frame.height};
    canvas[3].position = {0, frame.height};

    canvas[0].texCoords = {frame.left, frame.top};
    canvas[1].texCoords = {frame.left + frame.width, frame.top};
    canvas[2].texCoords = {frame.left + frame.width, frame.top + frame.height};
    canvas[3].texCoords = {frame.left, frame.top + frame.height};

    renderStates.transform = transformable.getTransform();
    renderStates.texture = sprite.getTexture();
}

SpriteSheetsManager::SpriteSheetsManager(const TexturesManager &texturesManager) : mTextures{} {
    load(texturesManager, SpriteSheetId::Bullet, TextureId::Bullet, sf::Vector2u(8, 8));
    load(texturesManager, SpriteSheetId::Bunker, TextureId::Bunker, sf::Vector2u(56, 56));
    load(texturesManager, SpriteSheetId::SpaceShip, TextureId::SpaceShip, sf::Vector2u(32, 32));
    load(texturesManager, SpriteSheetId::Supply, TextureId::Supply, sf::Vector2u(22, 22));
    load(texturesManager, SpriteSheetId::Terrain, TextureId::Terrain, sf::Vector2u(14, 1));
}

void SpriteSheetsManager::load(const TexturesManager &texturesManager, const SpriteSheetId spriteSheetId, const TextureId textureId, const sf::Vector2u frameSize) {
    auto &buffer = mSpriteSheets.at(helpers::enumValue(spriteSheetId));
    const auto &texture = texturesManager.getTexture(textureId);
    const auto[textureWidth, textureHeight] = texture.getSize();
    const auto columns = textureWidth / frameSize.x, rows = textureHeight / frameSize.y;

    if (columns * frameSize.x > textureWidth || rows * frameSize.y > textureHeight) {
        throw std::invalid_argument(trace("bad dimensions supplied"));
    }

    buffer.reserve(rows * columns);
    for (auto row = 0u; row < rows; ++row) {
        const auto top = row * frameSize.y;

        for (auto column = 0u; column < columns; ++column) {
            buffer.emplace_back(column * frameSize.x, top, frameSize.x, frameSize.y);
        }
    }

    mTextures.at(helpers::enumValue(spriteSheetId)) = &texture;
}
