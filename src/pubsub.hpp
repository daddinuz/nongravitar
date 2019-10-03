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
    class Handler {
    public:
        template<typename U>
        friend void publish(const U &message);

        template<typename U, typename ...Args>
        friend void publish(Args &&... args);

        template<typename U>
        friend void subscribe(const Handler<U> &handler);

        template<typename U>
        friend void unsubscribe(const Handler<U> &handler);

        virtual ~Handler();

    protected:
        virtual void operator()(const T &) noexcept = 0;

    private:
        inline static std::unordered_set<Handler<T> *> mHandlers; // mutex is not needed because we are single-threaded
    };

    template<typename T>
    void publish(const T &message) {
        for (auto handler : Handler<T>::mHandlers) {
            (*handler)(message);
        }
    }

    template<typename T, typename ...Args>
    void publish(Args &&... args) {
        auto message = T(std::forward<Args>(args)...);
        for (auto handler : Handler<T>::mHandlers) {
            (*handler)(message);
        }
    }

    template<typename T>
    void subscribe(const Handler<T> &handler) {
        Handler<T>::mHandlers.insert(const_cast<Handler<T> *>(&handler));
    }

    template<typename T>
    void unsubscribe(const Handler<T> &handler) {
        Handler<T>::mHandlers.erase(const_cast<Handler<T> *>(&handler));
    }

    template<typename T>
    Handler<T>::~Handler() {
        unsubscribe<T>(*this);
    }
}
