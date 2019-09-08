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
        using FrameBuffer = const std::vector<Frame>;
        using const_iterator = std::vector<Frame>::const_iterator;
        using const_reverse_iterator = std::vector<Frame>::const_reverse_iterator;

        SpriteSheet() = delete; // no default-constructible

        SpriteSheet(const SpriteSheet &) = delete; // no copy-constructible;
        SpriteSheet &operator=(const SpriteSheet &) = delete; // no copy-assignable;

        SpriteSheet(SpriteSheet &&) noexcept = default; // move-constructible;
        SpriteSheet &operator=(SpriteSheet &&) = delete; // no move-assignable;

        [[nodiscard]] static SpriteSheet
        from(const sf::Texture &texture, unsigned frameWidth, unsigned frameHeight,
             sf::Vector2u size, sf::Vector2u startCoord = {0, 0});

        [[nodiscard]] static SpriteSheet
        from(const sf::Texture &texture, unsigned frameWidth, unsigned frameHeight, sf::Vector2u startCoord = {0, 0});

        [[nodiscard]] const std::vector<Frame> *operator->() const noexcept;

        [[nodiscard]] const std::vector<Frame> &operator*() const noexcept;

        [[nodiscard]] const sf::Texture &getTexture() const noexcept;

        [[nodiscard]] const Frame &getFrame(const sf::Vector2u &frameCoord) const;

        [[nodiscard]] sf::Sprite getSprite(const sf::Vector2u &frameCoord) const;

        [[nodiscard]] const sf::Vector2u &getSize() const noexcept;

        [[nodiscard]] const_iterator getFrameIterator(const sf::Vector2u &frameCoord = {0, 0}) const;

        [[nodiscard]] const_reverse_iterator getReverseFrameIterator(const sf::Vector2u &frameCoord = {0, 0}) const;

    private:
        SpriteSheet(const sf::Texture &texture, std::vector<Frame> &&frames, sf::Vector2u size) noexcept;

        [[nodiscard]] std::size_t getFrameIndex(const sf::Vector2u &frameCoord) const noexcept;

        std::vector<Frame> mFrames;
        const sf::Texture &mTexture;
        sf::Vector2u mSize;
    };
}
