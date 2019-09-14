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

#include <SFML/Graphics.hpp>

namespace gravitar {
    class SpriteSheet final {
    public:
        using Frame = sf::IntRect;
        using Buffer = std::vector<Frame>;

        using const_iterator = Buffer::const_iterator;

        using difference_type = Buffer::difference_type;
        using value_type = Buffer::value_type;
        using pointer = Buffer::pointer;
        using reference = Buffer::reference;
        using iterator_category = std::forward_iterator_tag;

        SpriteSheet() = delete; // no default-constructible

        SpriteSheet(const SpriteSheet &) = delete; // no copy-constructible;
        SpriteSheet &operator=(const SpriteSheet &) = delete; // no copy-assignable;

        SpriteSheet(SpriteSheet &&) noexcept = default; // move-constructible;
        SpriteSheet &operator=(SpriteSheet &&) noexcept = delete; // no move-assignable;

        [[nodiscard]] static SpriteSheet from(const sf::Texture &texture, sf::Vector2u frame, sf::Vector2u startCoord = {0, 0});

        [[nodiscard]] const_iterator cbegin() const noexcept;

        [[nodiscard]] const_iterator cend() const noexcept;

        [[nodiscard]] const Buffer &getBuffer() const noexcept;

        [[nodiscard]] const sf::Texture &getTexture() const noexcept;

        [[nodiscard]] const Frame &getFrame(const sf::Vector2u &frameCoord) const;

        [[nodiscard]] sf::Sprite instanceSprite(const sf::Vector2u &frameCoord) const;

        [[nodiscard]] const sf::Vector2u &getSize() const noexcept;

    private:
        SpriteSheet(const sf::Texture &texture, Buffer &&buffer, sf::Vector2u size) noexcept;

        [[nodiscard]] std::size_t frameIndex(const sf::Vector2u &frameCoord) const noexcept;

        Buffer mBuffer;
        const sf::Texture &mTexture;
        sf::Vector2u mSize;
    };
}
