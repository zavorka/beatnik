#pragma once

#include <functional>
#include <complex>
#include <array>

#include "../../log.h"
#include "../../external/kiss_fft/kiss_fftr.h"

namespace reBass {
    template <typename T> using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

    template <std::size_t WindowLength>
    class Kiss_FFTR final {
        static constexpr std::size_t FFT_LENGTH = WindowLength / 2 + 1;
    public:
        Kiss_FFTR() noexcept
            : fft_config {
                  kiss_fftr_alloc((int) WindowLength, 0, 0, 0),
                  kiss_fftr_free
              }
        {
        };

        const std::array<std::complex<float>, FFT_LENGTH>&
        compute_fft(
                const std::array<float, WindowLength>& buffer
        ) noexcept {
            // Yes, kiss_fft_cpx is the same data structure as complex<float>
            kiss_fftr(
                    fft_config.get(),
                    buffer.data(),
                    reinterpret_cast<kiss_fft_cpx*>(fft_buffer.data())
            );
            //normalize_frequency_data();
            return fft_buffer;
        };
        const std::array<std::complex<float>, FFT_LENGTH>&
        get_fft_buffer()
        const noexcept {
            return fft_buffer;
        };
    private:
        deleted_unique_ptr<kiss_fftr_state> fft_config;
        std::array<std::complex<float>, FFT_LENGTH> fft_buffer;

    };
}