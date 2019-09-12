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

#include <utility>

namespace gravitar {
    template<typename T>
    class Wrapper {
    public:
        template<typename ...Args>
        explicit Wrapper(Args &&... args);

        Wrapper(Wrapper<T> &&) noexcept = default; // move-constructible
        Wrapper &operator=(Wrapper<T> &&) noexcept = default; // move-assignable

        [[nodiscard]] const T &get() const noexcept;
        [[nodiscard]] T &get() noexcept;

        [[nodiscard]] const T &operator*() const noexcept;
        [[nodiscard]] T &operator*() noexcept;

        [[nodiscard]] const T *operator->() const noexcept;
        [[nodiscard]] T *operator->() noexcept;

        virtual ~Wrapper() = 0;

    private:
        T mInstance;
    };

    /*
     * Implementation
     */

    template<typename T>
    template<typename... Args>
    Wrapper<T>::Wrapper(Args &&... args) : mInstance(std::forward<Args>(args)...) {}

    template<typename T>
    const T &Wrapper<T>::get() const noexcept {
        return mInstance;
    }

    template<typename T>
    T &Wrapper<T>::get() noexcept {
        return mInstance;
    }

    template<typename T>
    const T &Wrapper<T>::operator*() const noexcept {
        return mInstance;
    }

    template<typename T>
    T &Wrapper<T>::operator*() noexcept {
        return this->mInstance;
    }

    template<typename T>
    const T *Wrapper<T>::operator->() const noexcept {
        return &mInstance;
    }

    template<typename T>
    T *Wrapper<T>::operator->() noexcept {
        return &this->mInstance;
    }

    template<typename T>
    Wrapper<T>::~Wrapper() = default;
}
