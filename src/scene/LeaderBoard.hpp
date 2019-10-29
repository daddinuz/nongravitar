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

#include <Scene.hpp>
#include <pubsub.hpp>
#include <messages.hpp>

namespace nongravitar::scene {
    class LeaderBoard final : public Scene,
                              public pubsub::Handler<messages::GameOver> {
    public:
        LeaderBoard() = default;

        LeaderBoard(const LeaderBoard &) = delete; // no copy-constructible
        LeaderBoard &operator=(const LeaderBoard &) = delete; // no copy-assignable

        LeaderBoard(LeaderBoard &&) = delete; // no move-constructible
        LeaderBoard &operator=(LeaderBoard &&) = delete; // no move-assignable

        SceneId onEvent(const sf::Event &event) final;

        SceneId update(const sf::RenderWindow &window, SceneManager &sceneManager, Assets &assets, sf::Time elapsed) final;

        void render(sf::RenderTarget &window) const final;

    private:
        Scene &setup(const sf::RenderWindow &window, Assets &assets) final;

        void operator()(const messages::GameOver &message) final;

        sf::Text mAction;
        sf::Text mScore;
    };
}
