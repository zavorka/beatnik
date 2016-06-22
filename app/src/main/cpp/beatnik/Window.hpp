/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM DSP library
    Centre for Digital Music, Queen Mary, University of London.
    This file Copyright 2006 Chris Cannam.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#pragma once

#include <cmath>
#include <iostream>
#include <map>
#include <vector>

namespace reBass {

    enum Window_type {
        rectangular,
        bartlett,
        hamming,
        hanning,
        blackman,
        blackman_harris,

        first = rectangular,
        last = blackman_harris
    };

/**
 * Various shaped windows for sample frame conditioning, including
 * cosine windows (Hann etc) and triangular and rectangular windows.
 */
    template<typename T>
    class Window {
    public:
        /**
         * Construct a windower of the given type and size.
         *
         * Note that the cosine windows are periodic by design, rather
         * than symmetrical. (A Window of size N is equivalent to a
         * symmetrical Window of size N+1 with the final element missing.)
         */
        Window(Window_type type, size_t size)
                : type(type), size(size), cache(size) { encache(); }

        template <class InputIterator, class OutputIterator>
        void cut(InputIterator input, OutputIterator result) const {
            for (size_t i = 0; i < size; i++) {
                *result = *input * cache[i];
                ++result; ++input;
            }
        };

        Window_type get_type() const { return type; }

        size_t get_size() const { return size; }

        std::vector<T> get_window_data() const {
            return cache;
        }

    private:
        Window_type type;
        size_t size;
        std::vector<T> cache;

        void encache();
    };

    template<typename T>
    void Window<T>::encache() {
        std::fill(cache.begin(), cache.end(), 1);
        size_t n = size;
        switch (type) {
            case rectangular:
                for (size_t i = 0; i < n; ++i) {
                    cache.at(i) = cache.at(i) * 0.5;
                }
                break;

            case bartlett:
                if (n == 2) {
                    cache[0] = cache[1] = 0; // "matlab compatible"
                } else if (n == 3) {
                    cache[0] = 0;
                    cache[1] = cache[2] = 2. / 3.;
                } else if (n > 3) {
                    for (size_t i = 0; i < n / 2; ++i) {
                        cache[i] = cache[i] * (i / T(n / 2));
                        cache[i + n - n / 2] =
                                cache[i + n - n / 2] * (1.0 - (i / T(n / 2)));
                    }
                }
                break;

            case hamming:
                if (n <= 1) break;
                for (size_t i = 0; i < n; ++i) {
                    cache[i] = cache[i] * (0.54 - 0.46 * cos(2 * M_PI * i / n));
                }
                break;

            case hanning:
                if (n <= 1) break;
                for (size_t i = 0; i < n; ++i) {
                    cache[i] = cache[i] * (0.50 - 0.50 * cos(2 * M_PI * i / n));
                }
                break;

            case blackman:
                if (n <= 1) break;
                for (size_t i = 0; i < n; ++i) {
                    cache[i] = cache[i] * (0.42 - 0.50 * cos(2 * M_PI * i / n)
                                           + 0.08 * cos(4 * M_PI * i / n));
                }
                break;

            case blackman_harris:
                for (size_t i = 0; i < n; ++i) {
                    cache[i] = cache[i] * (0.35875
                                           - 0.48829 * cos(2 * M_PI * i / n)
                                           + 0.14128 * cos(4 * M_PI * i / n)
                                           - 0.01168 * cos(6 * M_PI * i / n));
                }
                break;
        }
    }
}
