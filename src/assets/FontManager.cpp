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
#include <assets/FontManager.hpp>

using namespace gravitar::assets;

constexpr auto FONTS_PATH = GRAVITAR_DIRECTORY "/assets/fonts";

void FontManager::initialize() {
    std::array<const std::tuple<const char *, FontId>, 1> items = {
            std::make_tuple<const char *, FontId>("mechanical.otf", FontId::Mechanical),
    };

    for (const auto &i : items) {
        load(std::get<0>(i), std::get<1>(i));
    }
}

const sf::Font &FontManager::get(const FontId id) const noexcept {
    return mFonts.at(id);
}

void FontManager::load(const char *const filename, const FontId id) {
    char path[256];
    std::snprintf(path, std::size(path), "%s/%s", FONTS_PATH, filename);

    if (auto &soundtrack = mFonts[id]; !soundtrack.loadFromFile(path)) {
        std::snprintf(path, std::size(path), "%sUnable to load font: %s", __TRACE__, filename);
        throw std::runtime_error(path);
    }
}
