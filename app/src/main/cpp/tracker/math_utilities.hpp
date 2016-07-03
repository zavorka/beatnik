#pragma once
#include <vector>

#pragma GCC visibility push(hidden)

#ifndef PI
#define PI (3.14159265358979232846)
#endif

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
                std::size_t start,
                std::size_t count
        );

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
    };
}

#pragma GCC visibility pop
