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
    struct Animation : public sf::Drawable, public sf::Transformable {
        virtual void update(const sf::Time &elapsed) = 0;
    };

    class SpriteAnimation final : public Animation {
    public:
        SpriteAnimation() = delete;

        explicit SpriteAnimation(const sf::Texture &texture);

        SpriteAnimation(const sf::Texture &texture, sf::Vector2i coord, sf::Vector2i table, sf::Vector2i frame, sf::Time frameTime = sf::Time::Zero);

        SpriteAnimation(const SpriteAnimation &) = delete;

        const SpriteAnimation &operator=(const SpriteAnimation &) = delete;

        void update(const sf::Time &elapsed) final;

        sf::FloatRect getLocalBounds() const;

        sf::FloatRect getGlobalBounds() const;

        size_t addFrame(sf::IntRect frame);

        void setFrame(size_t frame, bool resetTime = true);

        const sf::IntRect &getFrame(size_t frame) const;

        const std::vector<sf::IntRect> &frames() const;

        void setFrameTime(sf::Time frameTime);

        const sf::Time &getFrameTime() const;

        void setLoop(bool loop);

    private:
        void draw(sf::RenderTarget &target, sf::RenderStates states) const final;

        std::vector<sf::IntRect> mFrames;
        const sf::Texture &mTexture;
        sf::Vertex mVertices[4];
        sf::Time mFrameTime;
        sf::Time mElapsed;
        size_t mCurrentFrame{0};
        bool mLoop{true};
    };
}
