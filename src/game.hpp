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

#include "assets.hpp"
#include <SFML/Graphics.hpp>

namespace gravitar {
    class Game final {
    public:
        enum class Scene {
            Curtain,
            SolarSystem,
            PlanetAssault,
        };

        Game() = default; // default-constructible

        Game(const Game &) = delete; // no copy-constructible
        Game &operator=(const Game &) = delete; // no copy-assignable

        Game(Game &&) = delete; // no move-constructible
        Game &operator=(Game &&) = delete; // no move-assignable

        Game &initialize();

        void update();

        void run();

    private:
        void updateCurtainScene();
        void updateSolarSystemScene();
        void updatePlanetAssaultScene();

        sf::RenderWindow mWindow;
        sf::Clock mTimer{};
        sf::Event mEvent{};
        Scene mScene{Scene::Curtain};
        FontsManager mFontsManager;
        SoundTracksManager mSoundTracksManager;
        SpriteSheetsManager mSpriteSheetsManager;
    };
}
