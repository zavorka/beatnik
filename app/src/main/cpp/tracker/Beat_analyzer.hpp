//
// Created by Roman Beránek on 24/06/2016.
//

#pragma once

#include <vector>
#include <deque>
#include <array>
#include <complex>
#include <stddef.h>
#include "../external/RealTime/RealTime.hpp"

#pragma GCC visibility push(default)

namespace reBass {


    struct Beat {
        RealTime timestamp;
        float bpm;
    };

    class Beat_analyzer {
    public:
        Beat_analyzer(unsigned int inputSampleRate, size_t step_size, size_t window_size);
        std::vector<Beat> get_beats();
        float get_bpm();
        void enqueue_df_value(const double df_value);
    protected:
        unsigned int sample_rate;
        size_t step_size;
        size_t window_size;

        std::deque<double> df;
    };

}

#pragma GCC visibility pop