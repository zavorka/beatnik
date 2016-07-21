//
// Created by Roman Beránek on 16/04/04.
//

#pragma once

#include <array>
#include <complex>
#include <vector>
#include <functional>
#include <memory>
#include <boost/circular_buffer.hpp>

#include "Kiss_FFTR.hpp"
#include "Hanning_window.hpp"

namespace reBass {
    template <std::size_t WindowLength>
    class FFT_rolling final
    {
        static_assert(WindowLength % 2 == 0, "Window length must be even.");
        static constexpr std::size_t FFT_LENGTH = WindowLength / 2 + 1;

    public:
        FFT_rolling() : FFT_rolling(WindowLength * 8) {};

        FFT_rolling(
                std::size_t history_buffer_size
        )
                : history_buffer(history_buffer_size)
        {
        };

        template <class InputIterator>
        const std::array<std::complex<float>, FFT_LENGTH>&
        compute_fft(InputIterator begin, InputIterator end)
        noexcept {
            history_buffer.insert(
                    history_buffer.end(),
                    begin,
                    end
            );
            if (history_buffer.size() < WindowLength) {
                history_buffer.insert(
                        history_buffer.begin(),
                        WindowLength - history_buffer.size(),
                        0.0f
                );
            }
            window.cut(
                    history_buffer.end() - WindowLength,
                    windowed_buffer.begin()
            );

            return fftr.compute_fft(windowed_buffer);
        };

        const std::array<std::complex<float>, FFT_LENGTH>&
        get_frequency_domain_data()
        const {
            return fftr.get_fft_buffer();
        };

        const std::array<float, FFT_LENGTH>&
        calculate_magnitudes()
        noexcept {
            auto &fft_buffer = fftr.get_fft_buffer();
            std::transform(
                    fft_buffer.cbegin(),
                    fft_buffer.cend(),
                    magnitudes_buffer.begin(),
                    [this] (const std::complex<float>& value) {
                        return std::abs(value * get_normalization_coefficient());
                    }
            );

            return magnitudes_buffer;
        };

    private:
        static constexpr float get_normalization_coefficient() {
            return 1.f / (WindowLength / 2 *
                        Hanning_window<float, WindowLength>
                            ::get_normalization_correction());
        };


        Kiss_FFTR<WindowLength> fftr;

        const Hanning_window<float, WindowLength> window;
        std::array<float, WindowLength> windowed_buffer;
        std::array<float, FFT_LENGTH> magnitudes_buffer;
        boost::circular_buffer<float> history_buffer;
    };
}
