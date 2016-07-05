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

        /**
         * Process a single frequency-domain frame, provided as
         * frameLength/2+1 real and imaginary component values.
         */
        double process_frequency_domain(
                const std::vector<std::complex<float>>& fft
        );

        constexpr static double DF_OUTPUT_VALUE_MULTIPLIER = 512;
    private:
        struct DFData
        {
            double magHistory = 0.0;
            double phaseHistory = 0.0;
            double phaseHistoryOld = 0.0;
            double magPeak = 0.0;

            double magnitude = 0.0;
            double phase;
            double unwrapped;

            DFData(double omega = 0): phase(-omega), unwrapped(-omega) { }
        };

        void whiten();
        double complex_spectral_difference();

        unsigned int frameLength;
        unsigned int stepSize;
        std::vector<DFData> data;
        static constexpr float kWhitenRelaxCoeff = 0.9997;
        static constexpr float kWhitenFloor = 0.01;

    };
}

//#pragma GCC visibility pop
