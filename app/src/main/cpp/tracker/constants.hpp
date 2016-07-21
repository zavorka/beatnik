#pragma once

namespace reBass {
    namespace constants {
        constexpr unsigned int SAMPLE_RATE = 44100;
        constexpr unsigned int FFT_STEP_SIZE = 128;
        constexpr unsigned int FFT_WINDOW_LENGTH = 512;
        constexpr unsigned int FFT_LENGTH = FFT_WINDOW_LENGTH / 2 + 1;
        constexpr float DF_OUTPUT_VALUE_MULTIPLIER = 512.f;
    }
}