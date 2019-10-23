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

#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>

namespace nongravitar {
    struct Sprite final {
        using Frame = sf::IntRect;

        const sf::Texture &texture;
        const Frame frame;
    };

    class SpriteSheet final {
    public:
        using Buffer = std::vector<Sprite::Frame>;

        SpriteSheet() = delete; // no default-constructible

        SpriteSheet(const sf::Texture &texture, sf::Vector2u frameSize);

        SpriteSheet(const SpriteSheet &) = delete; // no copy-constructible;
        SpriteSheet &operator=(const SpriteSheet &) = delete; // no copy-assignable;

        SpriteSheet(SpriteSheet &&) noexcept = default; // move-constructible;
        SpriteSheet &operator=(SpriteSheet &&) noexcept = default; // move-assignable;

        [[nodiscard]] inline std::size_t getSize() const noexcept {
            return mBuffer.size();
        }

        [[nodiscard]] inline Sprite getSprite(const std::size_t frameId) const {
            return {.texture=mTexture, .frame=getFrame(frameId)};
        }

        // TODO think about returning a reference
        [[nodiscard]] inline Sprite::Frame getFrame(const std::size_t frameId) const {
            return mBuffer.at(frameId);
        }

        [[nodiscard]] inline const sf::Texture &getTexture() const noexcept {
            return mTexture;
        }

        // TODO: complete remove this method
        [[nodiscard]] [[deprecated]] inline sf::Sprite instanceSprite(const std::size_t frameId) const {
            return {mTexture, getFrame(frameId)};
        }

    private:
        Buffer mBuffer;
        std::reference_wrapper<const sf::Texture> mTexture;
    };
}
