//
// Created by Roman Beránek on 7/14/16.
//

#include <iterator>
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
            ray_param((float) floor(tempo_to_lag_factor / DEFAULT_TEMPO)),
            counter(0),
            countdown(DF_LENGTH / DF_WINDOW),
            beat_period(tempo_to_lag_factor / DEFAULT_TEMPO),
            tempo(NAN),
            tempo_changed(false),
            should_calculate_periods(false),
            should_calculate_tempo(false),
            df_buffer(DF_LENGTH),
            rcf_processor(ray_param),
            decoder(ray_param),
            rcf_buffer(RCF_ROWS),
            background_thread([this] {
                thread_running.store(true);
                while (thread_running.load()) {
                    if (should_calculate_tempo) {
                        calculate_periods();
                        calculate_tempo();
                    }
                    std::unique_lock<std::mutex> cv_lock(cv_mutex);
                    cv.wait(cv_lock);
                }
            })
    {
        df_buffer.insert(
                df_buffer.begin(),
                df_buffer.capacity(),
                0.0f
        );

        rcf_buffer.insert(
                rcf_buffer.begin(),
                rcf_buffer.capacity(),
                std::array<float, RCF_COLUMNS>()
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
    BTrack::get_BPM()
    {
        return tempo.load();
    }

    bool
    BTrack::process_DF_sample(float sample) noexcept
    {
        return process_DF_samples({ sample });
    }

    bool
    BTrack::process_DF_samples(const std::vector<float> &samples)
    noexcept {
        std::unique_lock<std::mutex> df_lock(df_mutex);
        std::transform(
                samples.begin(),
                samples.end(),
                std::back_inserter(df_buffer),
                [](float sample) {
                    return sample + 0.0001f;
                }
        );
        df_lock.unlock();

        auto increment = samples.size();
        counter = (counter + increment) % DF_WINDOW;

        if (counter % DF_STEP < increment) {
            calculate_rcf();
        }

        if (counter % DF_WINDOW < increment) {
            should_calculate_tempo.store(true);
            std::unique_lock<std::mutex> cv_lock(cv_mutex);
            cv.notify_one();
            cv_lock.unlock();
        }

        auto changed = tempo_changed.load();
        tempo_changed.store(false);

        return changed;
    }

    void
    BTrack::calculate_rcf() noexcept
    {
        std::unique_lock<std::mutex> df_lock(df_mutex);
        auto rcf_row = rcf_processor.get_rcf(
                df_buffer.end() - DF_WINDOW
        );
        df_lock.unlock();

        std::unique_lock<std::mutex> rcf_lock(rcf_mutex);
        rcf_buffer.push_back(rcf_row);
        rcf_lock.unlock();
    }

    void
    BTrack::calculate_periods() noexcept
    {
        std::unique_lock<std::mutex> df_lock(df_mutex);
        std::copy(
                std::cbegin(df_buffer),
                std::cend(df_buffer),
                std::begin(df_copy)
        );
        df_lock.unlock();

        std::unique_lock<std::mutex> rcf_lock(rcf_mutex);
        std::copy(
                std::cbegin(rcf_buffer),
                std::cend(rcf_buffer),
                std::begin(rcf_matrix)
        );
        rcf_lock.unlock();

        periods = decoder.decode(rcf_matrix);
    }

    float
    BTrack::calculate_tempo() noexcept
    {
        cumulative_score.fill(0);
        backlink.fill(0);
        for (auto i = 0; i < RCF_ROWS; i++) {
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
                        ALPHA * max_score + (1. - ALPHA) * df_copy[i * DF_STEP + j];
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

        tempo.store(bpm);
        tempo_changed.store(true);

        return bpm;
    }
}

