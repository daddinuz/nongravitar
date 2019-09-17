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

#include <limits>
#include <type_traits>
#include <SFML/Graphics.hpp>
#include <assets/AudioManager.hpp>

namespace gravitar::scene {
    enum class SceneId : std::size_t {};
    constexpr auto NullScene = SceneId{std::numeric_limits<std::underlying_type<SceneId>::type>::max()};

    class Scene {
    public:
        friend class SceneManager;

        virtual void adjustAudio(assets::AudioManager &audioManager) noexcept;

        [[nodiscard]] virtual SceneId handleEvent(const sf::Event &event) noexcept;

        virtual void update(const sf::RenderWindow &window, sf::Time elapsed) noexcept;

        virtual void render(sf::RenderTarget &window) const noexcept = 0;

        [[nodiscard]] SceneId getId() const noexcept;

        virtual ~Scene() = default;

    private:
        SceneId mSceneId{NullScene};
    };
}
