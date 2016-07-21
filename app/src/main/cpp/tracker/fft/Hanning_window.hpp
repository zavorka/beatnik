#pragma once

#include <cmath>
#include <array>

namespace reBass
{
    template<typename T, std::size_t Length>
    class Hanning_window {
    public:
        Hanning_window()
        {
            for (size_t i = 0; i < Length; ++i) {
                cache[i] = 0.50f - 0.50f * (float) cos(2 * M_PI * i / Length);
            }
        }

        template <class InputIterator, class OutputIterator>
        void cut(InputIterator input, OutputIterator result) const {
            for (size_t i = 0; i < Length; i++) {
                *result = *input * cache[i];
                ++result; ++input;
            }
        };

        const std::array<T, Length> &get_window_data() const {
            return cache;
        }

        static constexpr float get_normalization_correction() {
            return 0.50f;
        }

    private:
        std::array<T, Length> cache;
    };
}
