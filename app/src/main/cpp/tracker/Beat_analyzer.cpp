//
// Created by Roman Beránek on 24/06/2016.
//

#include <math.h>
#include "Beat_analyzer.hpp"
#include "../log.h"

using std::vector;
using std::complex;

namespace reBass
{
    Beat_analyzer::Beat_analyzer(
            unsigned int sample_rate,
            unsigned int step_size
    )
            : sample_rate(sample_rate),
              step_size(step_size),
              df_length(MAX_BAR_DURATION_IN_SAMPLES / step_size),
              df(df_length),
              df_copy(df_length),
              beat_period(df_length),
              tracker(step_size, sample_rate, df_length)
    {
        df.insert(df.begin(), df.capacity(), 0.0);
    }

    void
    Beat_analyzer::enqueue_df_value(const double df_value)
    {
        df.push_back(df_value);
    }

    vector<double>
    Beat_analyzer::get_beats()
    {
        std::copy(df.begin(), df.end(), df_copy.begin());
        tracker.calculateBeatPeriod(df_copy, beat_period);
        LOGI("beats period: %lf", beat_period.back());
        return tracker.calculateBeats(df_copy, beat_period);
    }

    float
    Beat_analyzer::get_bpm() {
        return get_bpm(get_beats());
    }

    float
    Beat_analyzer::get_bpm(const std::vector<double> beats)
    {
        auto size = beats.size();
        LOGI("beats size: %d", size);

        if (size < 4) {
            return NAN;
        }

        auto beats_count = size - 3;
        auto frames_count = beats.back() - beats[2];

        float bpm =
                60.0f
                * (float) sample_rate
                / (float) step_size
                / (float) frames_count
                * (float) beats_count;

        while (bpm > 180.0) {
            bpm /= 2.0;
        }
        while (bpm < 90.0) {
            bpm *= 2.0;
        }

        /*
        while (bpm < 120.0) {
            bpm *= 1.5;
        }
        */

        return bpm;
    }

    void
    Beat_analyzer::clear_data() {
        df.clear();
    }
}
