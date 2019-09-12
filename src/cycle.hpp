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

namespace gravitar {
    template<typename Iterable, typename Iterator>
    class Cycle {
    public:
        using difference_type = typename Iterable::difference_type;
        using value_type = typename Iterable::value_type;
        using pointer = typename Iterable::pointer;
        using reference = typename Iterable::reference;
        using iterator_category = std::forward_iterator_tag;

        Cycle() = delete; // no default-constructible

        Cycle(Iterator begin, Iterator end);

        Cycle(const Cycle &) = delete; // no copy-constructible
        Cycle &operator=(const Cycle &) = delete; // no copy-assignable

        Cycle(Cycle &&) noexcept = default; // move-constructible
        Cycle &operator=(Cycle &&) noexcept = default; // move-assignable

        Cycle<Iterable, Iterator> &operator++();

        value_type operator*() const noexcept;

        bool operator==(const Cycle<Iterable, Iterator> &rhs) const noexcept;

        bool operator!=(const Cycle<Iterable, Iterator> &rhs) const noexcept;

    private:
        Iterator mCursor;
        Iterator mBegin;
        Iterator mEnd;
    };

    /*
     * Implementation
     */

    template<typename Iterable, typename Iterator>
    Cycle<Iterable, Iterator>::Cycle(Iterator begin, Iterator end) : mCursor(begin), mBegin(begin), mEnd(end) {}

    template<typename Iterable, typename Iterator>
    Cycle<Iterable, Iterator> &Cycle<Iterable, Iterator>::operator++() {
        if (++mCursor == mEnd) {
            mCursor = mBegin;
        }

        return *this;
    }

    template<typename Iterable, typename Iterator>
    typename Cycle<Iterable, Iterator>::value_type Cycle<Iterable, Iterator>::operator*() const noexcept {
        return *mCursor;
    }

    template<typename Iterable, typename Iterator>
    bool Cycle<Iterable, Iterator>::operator==(const Cycle<Iterable, Iterator> &rhs) const noexcept {
        return mCursor == rhs.mCursor;
    }

    template<typename Iterable, typename Iterator>
    bool Cycle<Iterable, Iterator>::operator!=(const Cycle<Iterable, Iterator> &rhs) const noexcept {
        return !operator==(rhs);
    }
}
