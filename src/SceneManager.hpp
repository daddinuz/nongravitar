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

#include <memory>
#include <vector>
#include <Scene.hpp>

namespace nongravitar {
    class SceneManager final {
    public:
        SceneManager() = default; // default-constructible

        SceneManager(const SceneManager &) = delete; // no copy-constructible
        SceneManager &operator=(const SceneManager &) = delete; // no copy-assignable

        SceneManager(SceneManager &&) = delete; // no move-constructible
        SceneManager &operator=(SceneManager &&) = delete; // no move-assignable

        template<typename T, typename ...Args>
        T &emplace(Args &&... args) {
            static_assert(std::is_base_of<Scene, T>::value);

            const auto id = SceneId{mScenes.size()};
            auto scene = std::make_unique<T>(std::forward<Args>(args)...);
            scene->mSceneId = id;
            mScenes.push_back(std::move(scene));

            return dynamic_cast<T &>(*mScenes.back());
        }

        Scene &get(SceneId id);

    private:
        std::vector<std::unique_ptr<Scene>> mScenes;
    };
}
