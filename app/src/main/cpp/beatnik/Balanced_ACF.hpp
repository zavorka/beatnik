//
// Created by Roman Beránek on 16/04/04.
//

#pragma once

#include <complex>
#include <vector>
#include <functional>
#include <memory>

#include "../external/kiss_fft/kiss_fftr.h"

namespace reBass {
    template <typename T> using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

    template <unsigned int N>
    class Balanced_ACF final {
    public:
        Balanced_ACF() :
                forward_config{
                        kiss_fftr_alloc(2 * N, 0, nullptr, nullptr),
                        kiss_fftr_free
                },
                backward_config{
                        kiss_fftr_alloc(2 * N, 1, nullptr, nullptr),
                        kiss_fftr_free
                }
        {

        };

        void
        perform_ACF(std::array<float, N> &buffer)
        {
            inverse_buffer.fill(0);

            std::copy(
                    buffer.cbegin(),
                    buffer.cend(),
                    inverse_buffer.begin()
            );

            kiss_fftr(
                    forward_config.get(),
                    inverse_buffer.data(),
                    reinterpret_cast<kiss_fft_cpx*>(fft_buffer.data())
            );

            std::for_each(
                    fft_buffer.begin(),
                    fft_buffer.end(),
                    [] (std::complex<float> &value) {
                        value = std::norm(value);
                    }
            );

            kiss_fftri(
                    backward_config.get(),
                    reinterpret_cast<kiss_fft_cpx*>(fft_buffer.data()),
                    inverse_buffer.data()
            );

            float lag = N;
            for (int i = 0; i < N; i++) {
                buffer[i] = std::fabsf(inverse_buffer[i]) / lag / N;
                lag--;
            }
        };

        void
        perform_non_FFT_ACF(std::array<float, N>& buffer) {
            for (auto lag = 0; lag < N; lag++) {
                float sum = 0;
                for (auto n = 0; n < (N - lag); n++) {
                    sum += buffer[n] * buffer[n + lag];
                }
                temp_acf[lag] = sum / (N - lag);
            }
            std::copy(
                    temp_acf.begin(),
                    temp_acf.end(),
                    buffer.begin()
            );
        }
    private:

        deleted_unique_ptr<kiss_fftr_state> forward_config;
        deleted_unique_ptr<kiss_fftr_state> backward_config;

        std::array<std::complex<float>, N + 1> fft_buffer;
        std::array<float, 2 * N> inverse_buffer;
        std::array<float, N> temp_acf;
    };
}
