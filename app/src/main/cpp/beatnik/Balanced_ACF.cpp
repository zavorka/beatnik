//
// Created by Roman Beránek on 7/14/16.
//

#include <cstdlib>
#include <stdexcept>
#include <string>

#include "Balanced_ACF.hpp"

using std::vector;
using std::complex;


namespace reBass
{
    Balanced_ACF::Balanced_ACF(size_t fft_length) :
            fft_length(fft_length),
            fft_in_buffer(fft_length),
            fft_out_buffer(fft_length),
            forward_config {
                    kiss_fft_alloc((int) fft_length, 0, nullptr, nullptr),
                    kiss_fft_free
            },
            backward_config {
                    kiss_fft_alloc((int) fft_length, 1, nullptr, nullptr),
                    kiss_fft_free
            }
    {

    }

    void Balanced_ACF::perform_ACF(std::vector<float> &buffer)
    {
        const auto buffer_size = std::min(buffer.size(), fft_length);

        for (int i = 0; i < fft_length; i++) {
            if (i < buffer_size) {
                fft_in_buffer[i] = buffer[i];
            } else {
                fft_in_buffer[i] = 0;
            }
        }

        kiss_fft(
                forward_config.get(),
                reinterpret_cast<kiss_fft_cpx*>(fft_in_buffer.data()),
                reinterpret_cast<kiss_fft_cpx*>(fft_out_buffer.data())
        );

        for (int i = 0; i < fft_length; i++) {
            fft_out_buffer[i] = std::norm(fft_out_buffer[i]);
        }

        kiss_fft(
                backward_config.get(),
                reinterpret_cast<kiss_fft_cpx*>(fft_out_buffer.data()),
                reinterpret_cast<kiss_fft_cpx*>(fft_in_buffer.data())
        );

        double lag = buffer_size;
        for (int i = 0; i < buffer_size; i++) {
            buffer[i] = (float) (std::abs(fft_in_buffer[i]) / lag / fft_length);
            lag--;
        }
    }
}
