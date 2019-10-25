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
#include <assets/TexturesManager.hpp>

using namespace nongravitar::assets;

TexturesManager::TexturesManager() {
    load("bullet.png", TextureId::Bullet);
    load("bunker.png", TextureId::Bunker);
    load("planet.png", TextureId::Planet);
    load("spaceship.png", TextureId::SpaceShip);
    load("supply.png", TextureId::Supply);
    load("terrain.png", TextureId::Terrain);
    load("title.png", TextureId::Title);
}

void TexturesManager::load(const char *filename, const TextureId textureId) {
    auto path = std::string(NONGRAVITAR_TEXTURES_PATH "/") + filename;

    if (auto &texture = mTextures[helpers::enumValue(textureId)]; texture.loadFromFile(path)) {
        texture.setSmooth(true);
    } else {
        path.insert(0, __TRACE__ "Unable to load texture: ");
        throw std::runtime_error(path);
    }
}
