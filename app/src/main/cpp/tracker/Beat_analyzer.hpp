//
// Created by Roman Beránek on 24/06/2016.
//

#pragma once

#include <vector>
#include <array>
#include <complex>
#include <stddef.h>
#include <boost/circular_buffer.hpp>
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
        float get_bpm(const std::vector<Beat>& beats);
        void enqueue_df_value(const double df_value);
        void clear_data();
    protected:
        // At 96,000 Hz sample rate, a bar at 90 bpm lasts 256k samples
        // 2^18 is therefore a power of two large just enough
        static constexpr unsigned int MAX_BAR_DURATION_IN_SAMPLES = 512 * 512;
        unsigned int sample_rate;
        size_t step_size;
        size_t window_size;

        boost::circular_buffer<double> df;
    };

}

#pragma GCC visibility pop