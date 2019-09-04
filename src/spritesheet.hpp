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
    class SpriteSheet : virtual public sf::Drawable, virtual public sf::Transformable {
    public:
        using Frame = sf::IntRect;
        using FrameId = size_t;

        SpriteSheet() = delete;

        explicit SpriteSheet(const sf::Texture &texture);

        const SpriteSheet &operator=(const SpriteSheet &) = delete;

        static SpriteSheet from(const sf::Texture &texture, sf::Vector2i startCoord, sf::Vector2i table, sf::Vector2i frame);

        void setFrame(FrameId frameId);

        template<typename... Args>
        FrameId addFrame(Args &&... args) {
            mFrames.emplace_back(args...);
            return mFrames.size() - 1;
        }

        FrameId addFrame(Frame frame);

        [[nodiscard]] FrameId getCurrentFrameId() const;

        [[nodiscard]] const Frame &getCurrentFrame() const;

        [[nodiscard]] const Frame &getFrame(FrameId frameId) const;

        [[nodiscard]] const std::vector<Frame> &getFrames() const;

        [[nodiscard]] sf::FloatRect getLocalBounds() const;

        [[nodiscard]] sf::FloatRect getGlobalBounds() const;

    protected:
        SpriteSheet(const SpriteSheet &) = default;

        SpriteSheet(const sf::Texture &texture, sf::Vector2i startCoord, sf::Vector2i table, sf::Vector2i frame);

        void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    private:
        std::vector<Frame> mFrames;
        sf::Sprite mSprite;
        FrameId mCurrentFrameId{std::numeric_limits<FrameId>::max()};
    };
}
