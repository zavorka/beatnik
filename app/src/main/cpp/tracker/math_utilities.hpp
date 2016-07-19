#pragma once
#include <vector>
#include <array>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <cmath>

#pragma GCC visibility push(hidden)

#ifndef PI
#define PI (3.14159265358979232846)
#endif

using std::size_t;
using std::cbegin;
using std::cend;

namespace reBass
{
    /**
     * Static helper functions for simple mathematical calculations.
     */
    namespace math_utilities
    {
        /**
         * Return the mean of the subset of the given vector identified by
         * start and count.
         */
        double mean(
                const std::vector<double> &data,
                size_t start,
                size_t count
        );

        float mean(
                const std::vector<float> &data,
                size_t start,
                size_t count
        );

        template <typename T, size_t N>
        double mean(
                const std::array<T, N> &data,
                size_t start,
                size_t length
        ) {
            double sum = std::accumulate(
                cbegin(data) + start,
                cbegin(data) + start + length,
                0
            );
            return sum / length;
        };

        template <typename T, size_t N>
        double mean(
                const std::array<T, N> &data
        ) {
            return mean(data, 0, N);
        };

        /**
         * The principle argument function. Map the phase angle ang into
         * the range [-pi,pi).
         */
        double princarg( double ang );

        /**
         * Threshold the input/output vector data against a moving-mean
         * average filter.
         */
        void adaptive_threshold(std::vector<double> &data);
        void adaptive_threshold(std::vector<float> &data);

        template <typename T, size_t N>
        void adaptive_threshold(std::array<T, N> &array)
        {
            T thresh[N];

            const size_t p_post = 7;
            const size_t p_pre = 8;

            size_t i;

            for (i = 0; i < N; i++) {
                size_t first = std::max((size_t) 0, i - p_pre);
                size_t last  = std::min(N - 1,      i + p_post);

                thresh[i] = mean(array, first, last - first + 1);
            }


            for (i = 0; i < N; i++) {
                array[i] = std::max(array[i] - thresh[i], (T) 0);
            }
        };

        void normalize(std::vector<double> &data);

        template <typename T, size_t N>
        void normalize(std::array<T, N> &data) {
            T sum = 0;
            for (auto i = 0; i < N; i++) {
                if (data[i] > 0) {
                    sum += data[i];
                }
            }
            if (sum <= 0) {
                return;
            }
            for (auto i = 0; i < N; i++) {
                data[i] /= sum;
            }
        };

        template <size_t N>
        void comb_filter(
                const std::array<float, N> &input,
                const std::array<double, N/4> &weighting_vector,
                std::array<double, N/4> &output
        ) {
            output.fill(0.);
            for (auto i = 2; i < N/4; i++) {
                for (auto a = 1; a <= 4; a++) {
                    for (auto b = 1 - a; b <= a - 1; b++) {
                        output[i - 1] =
                                output[i - 1]
                                + (input[a * i + b - 1]
                                   * weighting_vector[i - 1]) / (2 * a - 1);
                    }
                }
            }
        }
    };
}

#pragma GCC visibility pop
