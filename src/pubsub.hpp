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

#include <unordered_set>

namespace gravitar::pubsub {
    template<typename T>
    class Listen {
    public:
        template<typename U>
        friend void notify(const U &);

        template<typename U, typename ...Args>
        friend void notify(Args &&...);

        template<typename U>
        friend void subscribe(Listen<U> &);

        template<typename U>
        friend void unsubscribe(Listen<U> &);

        virtual void onNotify(const T &) noexcept = 0;

        virtual ~Listen();

    private:
        inline static std::unordered_set<Listen<T> *> mListeners; // mutex is not needed until we are single-threaded
    };

    template<typename T>
    void notify(const T &message) {
        for (auto listener : Listen<T>::mListeners) {
            listener->onNotify(message);
        }
    }

    template<typename T, typename ...Args>
    void notify(Args &&... args) {
        auto message = T(std::forward<Args>(args)...);
        for (auto listener : Listen<T>::mListeners) {
            listener->onNotify(message);
        }
    }

    template<typename T>
    void subscribe(Listen<T> &listener) {
        Listen<T>::mListeners.insert(&listener);
    }

    template<typename T>
    void unsubscribe(Listen<T> &listener) {
        Listen<T>::mListeners.erase(&listener);
    }

    template<typename T>
    Listen<T>::~Listen() {
        unsubscribe<T>(*this);
    }
}
