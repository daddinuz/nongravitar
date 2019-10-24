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

#include <array>
#include <helpers.hpp>
#include <SFML/Graphics.hpp>

namespace nongravitar::assets {
    enum class FontId : std::size_t {
        Mechanical = 0,
    };

    class FontsManager final {
    public:
        FontsManager();

        FontsManager(const FontsManager &) = delete; // no copy-constructible
        FontsManager &operator=(const FontsManager &) = delete; // no copy-assignable

        FontsManager(FontsManager &&) = delete; // no move-constructible
        FontsManager &operator=(FontsManager &&) = delete; // no move-assignable

        [[nodiscard]] inline const sf::Font &getFont(const FontId id) const {
            return mFonts.at(helpers::enumValue(id));
        }

    private:
        void load(const char *filename, FontId id);

        std::array<sf::Font, 1> mFonts;
    };
}
