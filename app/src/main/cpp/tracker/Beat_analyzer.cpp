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
              window_size(window_size)
    {
    }

    void
    Beat_analyzer::enqueue_df_value(const double df_value)
    {
        if (df.size() >= 2048) {
            df.erase(df.begin());
        }

        df.push_back(df_value);
    }

    vector<Beat>
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
            return vector<Beat>();
        }

        vector<double> df_vector {
                df.cbegin() + 2,
                df.cbegin() + nonZeroCount
        };
        vector<double> beatPeriod(nonZeroCount - 2);

        Tracker tracker(step_size, sample_rate);
        tracker.calculateBeatPeriod(df_vector, beatPeriod);

        vector<double> beats;
        tracker.calculateBeats(df_vector, beatPeriod, beats);

        vector<Beat> returnBeats;

        for (size_t i = 0; i < beats.size(); ++i)
        {
            size_t frame = (size_t) (beats[i] * step_size);
            Beat beat;
            beat.timestamp = RealTime::frame2RealTime(
                    (int)frame,
                    sample_rate
            );

            int frameIncrement = 0;

            if (i + 1 < beats.size())
            {
                frameIncrement = (int) ((beats[i + 1] - beats[i]) * step_size);
                if (frameIncrement > 0) {
                    float bpm = (60.0f * sample_rate) / frameIncrement;
                    beat.bpm = int(bpm * 100.0f + 0.5f) / 100.0f;
                }
            }

            returnBeats.push_back(beat); // beats are output 0
        }

        return returnBeats;
    }

    float
    Beat_analyzer::get_bpm() {
        auto bpms = get_beats();
        size_t size = bpms.size();
        if (size < 1) {
            return NAN;
        }

        RealTime timespan = (bpms.back().timestamp - bpms.front().timestamp);
        float seconds = timespan.sec + timespan.nsec/1.0E9f;
        float bpm = 60 * (size - 1) / seconds;

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
