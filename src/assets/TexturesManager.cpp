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

using namespace gravitar::assets;

constexpr auto TEXTURES_PATH = GRAVITAR_DIRECTORY "/assets/textures";

void TexturesManager::initialize() {
    std::array<const std::tuple<const char *, TextureId>, 3> items = {
            std::make_tuple<const char *, TextureId>("gravitar-title.png", TextureId::GravitarTitle),
            std::make_tuple<const char *, TextureId>("spaceship.png", TextureId::SpaceShip),
            std::make_tuple<const char *, TextureId>("bullet.png", TextureId::Bullet),
    };

    for (const auto &i : items) {
        load(std::get<0>(i), std::get<1>(i));
    }
}

const sf::Texture &TexturesManager::get(TextureId id) const noexcept {
    return mTextures.at(id);
}

void TexturesManager::load(const char *filename, TextureId id) {
    char path[256];
    std::snprintf(path, std::size(path), "%s/%s", TEXTURES_PATH, filename);

    if (auto &texture = mTextures[id]; texture.loadFromFile(path)) {
        texture.setSmooth(true);
    } else {
        std::snprintf(path, std::size(path), "%sUnable to load texture: %s", __TRACE__, filename);
        throw std::runtime_error(path);
    }
}
