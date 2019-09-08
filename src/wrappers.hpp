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
    template<typename T>
    class TransformableDrawable final : public sf::Drawable {
    public:
        static_assert(std::is_base_of<sf::Drawable, T>::value and std::is_base_of<sf::Transformable, T>::value);

        TransformableDrawable() = delete;

        explicit TransformableDrawable(T &&inner) noexcept;

        TransformableDrawable(const TransformableDrawable &) = delete;
        TransformableDrawable &operator=(const TransformableDrawable &) = delete;

        TransformableDrawable(TransformableDrawable &&) = delete;
        TransformableDrawable &operator=(TransformableDrawable &&) = delete;

        [[nodiscard]] sf::Transformable &operator*() noexcept;

        [[nodiscard]] const sf::Transformable &operator*() const noexcept;

        [[nodiscard]] sf::Transformable *operator->() noexcept;

        [[nodiscard]] const sf::Transformable *operator->() const noexcept;

    private:
        void draw(sf::RenderTarget &target, sf::RenderStates states) const final;

        T mInner;
    };

    /*
     * Implementation
     */

    template<typename T>
    TransformableDrawable<T>::TransformableDrawable(T &&inner) noexcept : mInner(std::move(inner)) {}

    template<typename T>
    sf::Transformable &TransformableDrawable<T>::operator*() noexcept {
        return mInner;
    }

    template<typename T>
    const sf::Transformable &TransformableDrawable<T>::operator*() const noexcept {
        return mInner;
    }

    template<typename T>
    sf::Transformable *TransformableDrawable<T>::operator->() noexcept {
        return &mInner;
    }

    template<typename T>
    const sf::Transformable *TransformableDrawable<T>::operator->() const noexcept {
        return &mInner;
    }

    template<typename T>
    void TransformableDrawable<T>::draw(sf::RenderTarget &target, sf::RenderStates states) const {
        (void) states;
        target.draw(mInner);
    }
}
