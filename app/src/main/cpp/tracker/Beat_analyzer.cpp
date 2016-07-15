//
// Created by Roman Beránek on 24/06/2016.
//

#include <math.h>
#include "Beat_analyzer.hpp"
#include "Tracker.hpp"

using std::vector;
using std::complex;

namespace reBass
{
    Beat_analyzer::Beat_analyzer(
            unsigned int sample_rate,
            size_t step_size,
            size_t window_size
    )
            : sample_rate(sample_rate),
              step_size(step_size),
              window_size(window_size),
              df(MAX_BAR_DURATION_IN_SAMPLES / step_size)
    {
    }

    void
    Beat_analyzer::enqueue_df_value(const double df_value)
    {
        df.push_back(df_value);
    }

    vector<double>
    Beat_analyzer::get_beats()
    {
        size_t nonZeroCount = df.size();
        while (nonZeroCount > 0) {
            if (df[nonZeroCount - 1] > 0.0) {
                break;
            }
            --nonZeroCount;
        }

        if (nonZeroCount <= 2) {
            return vector<double>();
        }

        vector<double> df_vector {
                df.begin() + 2,
                df.begin() + nonZeroCount
        };
        vector<double> beatPeriod(nonZeroCount - 2);

        Tracker tracker(step_size, sample_rate);
        tracker.calculateBeatPeriod(df_vector, beatPeriod);

        vector<double> beats;
        tracker.calculateBeats(df_vector, beatPeriod, beats);

        return beats;
    }

    float
    Beat_analyzer::get_bpm() {
        return get_bpm(get_beats());
    }

    float
    Beat_analyzer::get_bpm(const std::vector<double> &beats)
    {
        size_t size = beats.size();
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

        while (bpm < 120.0) {
            bpm *= 1.5;
        }

        return bpm;
    }

    void
    Beat_analyzer::clear_data() {
        df.clear();
    }
}
