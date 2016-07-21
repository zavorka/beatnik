#pragma once
#include <array>
#include <iterator>
#include <complex>

#include "math_utilities.hpp"

namespace reBass
{
    template <std::size_t FFTLength>
    class Broadband_DF final
    {
    public:
        float
        process_magnitudes(
                const std::array<float, FFTLength>& magnitudes
        ) noexcept {
            std::copy(
                    std::cbegin(magnitudes),
                    std::cend(magnitudes),
                    std::begin(this->magnitudes)
            );
            //whiten();
            return evaluate_broadband_rise();
        };

    private:
        static constexpr float DB_RISE = 3.0f;
        static constexpr float RELAXATION_COEFFICIENT = 0.9997f;
        static constexpr float WHITEN_FLOOR = 0.01f;

        void
        whiten()
        noexcept {
            for (std::size_t i = 0; i < FFTLength; ++i) {
                float m = magnitudes[i];
                if (m < peak_magnitude[i]) {
                    m = m + (peak_magnitude[i] - m) * RELAXATION_COEFFICIENT;
                }
                if (m < WHITEN_FLOOR) {
                    m = WHITEN_FLOOR;
                }
                peak_magnitude[i] = m;
                magnitudes[i] /= m;
            }
        };

        float
        evaluate_broadband_rise()
        noexcept {
            float val = 0;

            for (std::size_t i = 0; i < FFTLength; i++) {
                float squared_magnitude = magnitudes[i] * magnitudes[i];
                if (previous_magnitude[i] > 0.0f) {
                    float diff = 10.0f * log10f(squared_magnitude / previous_magnitude[i]);
                    if (diff > DB_RISE) {
                        val = val + 1.0f;
                    }
                }
                previous_magnitude[i] = squared_magnitude;
            }

            return val;
        };

        std::array<float, FFTLength> magnitudes;
        std::array<float, FFTLength> previous_magnitude;
        std::array<float, FFTLength> peak_magnitude;
    };
}
