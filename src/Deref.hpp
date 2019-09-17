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
    class Deref {
    public:
        template<typename ...Args>
        explicit Deref(Args &&... args);

        Deref(Deref<T> &&) noexcept = default; // move-constructible
        Deref &operator=(Deref<T> &&) noexcept = default; // move-assignable

        [[nodiscard]] const T &deref() const noexcept;
        [[nodiscard]] T &deref() noexcept;

        [[nodiscard]] const T &operator*() const noexcept;
        [[nodiscard]] T &operator*() noexcept;

        [[nodiscard]] const T *operator->() const noexcept;
        [[nodiscard]] T *operator->() noexcept;

        virtual ~Deref() = 0;

    private:
        T mInstance;
    };

    /*
     * Implementation
     */

    template<typename T>
    template<typename... Args>
    Deref<T>::Deref(Args &&... args) : mInstance(std::forward<Args>(args)...) {}

    template<typename T>
    const T &Deref<T>::deref() const noexcept {
        return mInstance;
    }

    template<typename T>
    T &Deref<T>::deref() noexcept {
        return mInstance;
    }

    template<typename T>
    const T &Deref<T>::operator*() const noexcept {
        return mInstance;
    }

    template<typename T>
    T &Deref<T>::operator*() noexcept {
        return this->mInstance;
    }

    template<typename T>
    const T *Deref<T>::operator->() const noexcept {
        return &mInstance;
    }

    template<typename T>
    T *Deref<T>::operator->() noexcept {
        return &this->mInstance;
    }

    template<typename T>
    Deref<T>::~Deref() = default;
}
