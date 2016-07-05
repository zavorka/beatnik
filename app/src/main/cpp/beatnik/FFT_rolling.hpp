//
// Created by Roman Beránek on 16/04/04.
//

#pragma once

#include <complex>
#include <vector>
#include <functional>
#include <memory>
#include <boost/circular_buffer.hpp>

#include "Window.hpp"
#include "../external/kiss_fft/kiss_fftr.h"

namespace reBass {
    template <typename T> using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

    class FFT_rolling final {
    public:
        typedef std::function<void (const std::vector<std::complex<float>>&)> FFT_callback;
        FFT_rolling(size_t window_size);
        FFT_rolling(size_t window_size, size_t history_buffer_size);

        const std::vector<std::complex<float>>&
            compute_fft(const std::vector<float>& buffer);

        const std::vector<std::complex<float>>&
                get_frequency_domain_data()
                const;

        const std::vector<float>& calculate_magnitudes();
    private:
        size_t get_frequency_data_buffer_size() const;
        void normalize_frequency_data();

        const size_t window_size;
        const Window<float> window;
        std::vector<float> windowed_buffer;

        FFT_callback* callback;
        deleted_unique_ptr<kiss_fftr_state> fft_config;
        std::vector<std::complex<float>> fft_buffer;
        std::vector<float> magnitudes_buffer;
        boost::circular_buffer<float> history_buffer;

        const float normalization_coefficient;
    };
}
