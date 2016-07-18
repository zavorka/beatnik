//
// Created by Roman Beránek on 7/14/16.
//

#include <cmath>
#include "BTrack.hpp"
#include "math_utilities.hpp"

using std::pow;
using std::log;
using std::exp;
using std::fabsf;

namespace reBass
{
    BTrack::BTrack(unsigned int sample_rate,
                   unsigned int step_size) :
            sample_rate(sample_rate),
            step_size(step_size),
            tempo_to_lag_factor((60. * sample_rate) / step_size),
            ray_param(floor(tempo_to_lag_factor / DEFAULT_TEMPO)),
            counter(0),
            countdown(DF_LENGTH / DF_WINDOW),
            beat_period(tempo_to_lag_factor / DEFAULT_TEMPO),
            df_buffer(DF_LENGTH),
            rcf_processor(ray_param),
            decoder(ray_param),
            rcf_buffer(RCF_ROWS)
    {
        df_buffer.insert(
                df_buffer.begin(),
                df_buffer.capacity(),
                0.0f
        );

        rcf_buffer.insert(
                rcf_buffer.begin(),
                rcf_buffer.capacity(),
                std::array<double, RCF_COLUMNS>()
        );

        for (auto i = 0; i <= df_buffer.size(); i++) {
            if (i % ((int) round(tempo_to_lag_factor / DEFAULT_TEMPO)) == 0) {
                df_buffer[i] = 1;
            }
        }

        beats.reserve(DF_LENGTH);

        for (auto i = 1; i <= 128; i++) {
            period_constants.push_back(i);
        }
    }

    float
    BTrack::process_DF_sample(float sample)
    {
        // we need to ensure that the onset
        // detection function sample is positive
        sample = fabsf(sample) + 0.000001f;

        df_buffer.push_back(sample);

        if (counter % DF_STEP) {
            rcf_buffer.push_back(
                    rcf_processor.get_rcf(
                            df_buffer.end() - DF_WINDOW
                    )
            );
        }


        if (counter == DF_WINDOW) {
            counter = 0;
        } else {
            counter++;
        }

        if (counter == 1) {
            if (countdown <= 0) {
                return calculate_tempo();
            } else {
                countdown--;
            }
        }

        return 0.f;
    }

    float
    BTrack::calculate_tempo()
    {
        std::copy(
                rcf_buffer.begin(),
                rcf_buffer.end(),
                rcf_matrix.begin()
        );
        auto periods = decoder.decode(rcf_matrix);
        auto average_period = math_utilities::mean(periods);

        for (auto i = 0; i < periods.size(); i++) {
            auto &p = period_constants[periods[i] - 1];

            for (auto j = 0; j < DF_STEP; j++) {
                auto max_score = 0.;
                auto max_index = -1;

                for (auto k = 0; k < p.range_length; k++) {
                    int cscore_index = i * DF_STEP + j + p.min_range + k;
                    if (cscore_index >= 0) {
                        double score = p.txwt[k] * cumulative_score[cscore_index];
                        if (score > max_score) {
                            max_score = score;
                            max_index = k;
                        }
                    }
                }

                cumulative_score[i * DF_STEP + j] =
                        ALPHA * max_score + (1. - ALPHA) * df_buffer[i * DF_STEP + j];
                backlink[i * DF_STEP + j] = i * DF_STEP + j + p.min_range + max_index;
            }
        }

        beats.clear();

        auto period = periods.back();
        auto max_score = cumulative_score[DF_LENGTH - period];
        auto max_index = 0;
        for (auto i = DF_LENGTH - period; i < DF_LENGTH; i++) {
            if (cumulative_score[i] > max_score) {
                max_score = cumulative_score[i];
                max_index = i;
            }
        }

        beats.push_back(max_index);
        while (backlink[beats.back()] > 0) {
            int b = beats.back();
            if (backlink[b] == b) break;
            beats.push_back(backlink[b]);
        }

        if (beats.size() < 4) {
            return NAN;
        }

        auto beats_count = beats.size() - 3;
        auto frames_count = beats.front() - beats[beats_count];

        float bpm =
                (float) tempo_to_lag_factor
                / (float) frames_count
                * (float) beats_count;

        while (bpm > MAX_TEMPO) {
            bpm /= 2.0;
        }
        while (bpm < MIN_TEMPO) {
            bpm *= 2.0;
        }

        return bpm;
    }
}

