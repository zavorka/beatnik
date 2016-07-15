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

    class Balanced_ACF final {
    public:
        Balanced_ACF(size_t fft_length);

        void perform_ACF(std::vector<float>& buffer);
    private:

        const size_t fft_length;

        deleted_unique_ptr<kiss_fft_state> forward_config;
        deleted_unique_ptr<kiss_fft_state> backward_config;

        std::vector<std::complex<float>> fft_in_buffer;
        std::vector<std::complex<float>> fft_out_buffer;
    };
}
