#pragma once
#include <vector>
#include <complex>

#include "math_utilities.hpp"

//#pragma GCC visibility push(hidden)

namespace reBass
{
    class CSD_detection_function final
    {
    public:
        CSD_detection_function(unsigned int frameLength, unsigned int stepSize);

        float process_magnitudes(
                const std::vector<float>& magnitudes
        );

        constexpr static double DF_OUTPUT_VALUE_MULTIPLIER = 512;
        constexpr static double DB_RISE = 3.0;
    private:
        void whiten();
        float broadband();

        unsigned int frameLength;
        unsigned int stepSize;

        std::vector<float> magnitude;
        std::vector<float> previous_magnitude;
        std::vector<float> peak_magnitude;

        static constexpr float kWhitenRelaxCoeff = 0.9997;
        static constexpr float kWhitenFloor = 0.01;

    };
}

//#pragma GCC visibility pop
