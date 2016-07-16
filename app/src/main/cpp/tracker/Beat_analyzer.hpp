//
// Created by Roman Beránek on 24/06/2016.
//

#pragma once

#include <vector>
#include <array>
#include <complex>
#include <stddef.h>
#include <boost/circular_buffer.hpp>

#include "Tracker.hpp"

#pragma GCC visibility push(default)

namespace reBass {

    class Beat_analyzer {
    public:
        Beat_analyzer(unsigned int sample_rate,
                      unsigned int step_size);
        std::vector<double> get_beats();
        float get_bpm();
        float get_bpm(const std::vector<double> periods);
        void enqueue_df_value(const double df_value);
        void clear_data();
    protected:
        // At 96,000 Hz sample rate, a bar at 90 bpm lasts 256k samples
        // 2^18 is therefore a power of two large just enough
        static constexpr unsigned int MAX_BAR_DURATION_IN_SAMPLES = 512 * 512;
        const unsigned int sample_rate;
        const unsigned int step_size;
        const unsigned int df_length;

        boost::circular_buffer<double> df;
        std::vector<double> df_copy;
        std::vector<double> beat_period;
        Tracker tracker;
    };

}

#pragma GCC visibility pop