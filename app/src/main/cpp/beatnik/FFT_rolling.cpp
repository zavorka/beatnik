//
// Created by Roman Beránek on 16/04/04.
//

#include <cstdlib>
#include <stdexcept>
#include <string>

#include "FFT_rolling.hpp"

using std::vector;
using std::complex;


namespace reBass
{
    FFT_rolling::FFT_rolling(
            size_t window_size
    ) : FFT_rolling(window_size, window_size * 8) {
    }

    FFT_rolling::FFT_rolling(
            size_t window_size,
            size_t history_buffer_size
    )
            : window_size { window_size },
              window { Window_type::hanning, window_size },
              windowed_buffer(window_size),
              callback { nullptr },
              fft_config {
                      kiss_fftr_alloc((int) window_size, 0, 0, 0),
                      kiss_fftr_free
              },
              fft_buffer(get_frequency_data_buffer_size()),
              magnitudes_buffer(get_frequency_data_buffer_size()),
              history_buffer(history_buffer_size)
    {
        if (window_size % 2 != 0) {
            throw std::invalid_argument(
                    std::string("Window size must be even.")
            );
        }
    }

    const vector<complex<float>>&
    FFT_rolling::compute_fft(const vector<float> &buffer)
    {
        history_buffer.insert(
                history_buffer.end(),
                buffer.begin(),
                buffer.end()
        );
        if (history_buffer.size() < window_size) {
            history_buffer.insert(
                    history_buffer.begin(),
                    window_size - history_buffer.size(),
                    0.0f
            );
        }
        window.cut(history_buffer.end() - window_size, windowed_buffer.begin());

        // Yes, kiss_fft_cpx is the same data structure as complex<float>
        kiss_fftr(
                fft_config.get(),
                &windowed_buffer[0],
                reinterpret_cast<kiss_fft_cpx*>(&fft_buffer[0])
        );
        normalize_frequency_data();

        std::transform(
                fft_buffer.begin(),
                fft_buffer.end(),
                magnitudes_buffer.begin(),
                [] (const complex<float>& value) {
                    return std::abs(value);
                }
        );

        if (callback != nullptr) {
            (*callback)(fft_buffer);
        }
        return fft_buffer;
    }

    const vector<complex<float>>&
    FFT_rolling::compute_fft(const float * const buffer, size_t length)
    {
        history_buffer.insert(
                history_buffer.end(),
                buffer,
                buffer + length
        );
        if (history_buffer.size() < window_size) {
            history_buffer.insert(
                    history_buffer.begin(),
                    window_size - history_buffer.size(),
                    0.0f
            );
        }
        window.cut(history_buffer.end() - window_size, windowed_buffer.begin());

        // Yes, kiss_fft_cpx is the same data structure as complex<float>
        kiss_fftr(
                fft_config.get(),
                &windowed_buffer[0],
                reinterpret_cast<kiss_fft_cpx*>(&fft_buffer[0])
        );
        normalize_frequency_data();

        if (callback != nullptr) {
            (*callback)(fft_buffer);
        }
        return fft_buffer;
    }

    void FFT_rolling::normalize_frequency_data()
    {
        float correction = 1.f
             / (window_size / 2 * window.get_normalization_correction());
        std::for_each(
            fft_buffer.begin(),
            fft_buffer.end(),
            [correction] (std::complex<float> &value) {
                value *= correction;
            }
        );
    }

    size_t FFT_rolling::get_frequency_data_buffer_size() const {
        return window_size / 2 + 1;
    }

    const vector<complex<float>>&
    FFT_rolling::get_frequency_domain_data() const {
        return fft_buffer;
    }

    const vector<float>&
    FFT_rolling::get_magnitudes() const {
        return magnitudes_buffer;
    }
}
