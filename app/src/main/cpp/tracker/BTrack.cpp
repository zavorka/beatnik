//
// Created by Roman Beránek on 7/14/16.
//

#include <cmath>
#include "BTrack.hpp"
#include "math_utilities.hpp"

namespace reBass {
    BTrack::BTrack(unsigned int sample_rate,
                   unsigned int step_size) :
            sample_rate(sample_rate),
            step_size(step_size),
            tempo_to_lag_factor(60. * sample_rate / 32.),
            estimated_tempo(0),
            m0(10),
            beat_period((60. / step_size) * sample_rate / DEFAULT_TEMPO),
            beat_counter(-1),
            beat_due_in_frame(false),
            df_buffer(512 * 512 / step_size),
            cumulative_score(512 * 512 / step_size),
            resampled_DF(RESAMPLED_DF_LENGTH),
            acf(ACF_FFT_LENGTH),
            delta(DELTA_SIZE)
    {
        for (int n = 0; n < weighting_vector.size(); n++) {
            weighting_vector[n] =
                    ((double) n / std::pow(RAY_PARAM, 2.))
                    * std::exp(-1. * n * n / (2. * std::pow(RAY_PARAM, 2.0)));
        }

        previous_delta.fill(1);

        for (auto i = 0; i < DELTA_SIZE; i++) {
            for (auto j = 0; j < DELTA_SIZE; j++) {
                transition_matrix[i][j] =
                        (1. / (M_SIG * sqrt(2 * PI)))
                        * std::exp((-1. * (j - i) * (j - i)) / (2. * M_SIG * M_SIG));
            }
        }

        df_buffer.insert(
                df_buffer.begin(),
                df_buffer.capacity(),
                0.0
        );
        cumulative_score.insert(
                cumulative_score.begin(),
                cumulative_score.capacity(),
                0.0
        );
        for (auto i = 0; i < df_buffer.size(); i++) {
            if (i % ((int) round(beat_period)) == 0) {
                df_buffer[i] = 1;
            }
        }
    }

    double
    BTrack::process_DF_sample(double sample)
    {
        // we need to ensure that the onset
        // detection function sample is positive
        sample = std::fabs(sample);

        // add a tiny constant to the sample to stop it from ever going
        // to zero. this is to avoid problems further down the line
        sample += 0.0001;

        m0--;
        beat_counter--;
        beat_due_in_frame = false;

        df_buffer.push_back(sample);

        update_cumulative_score(sample);

        // if we are halfway between beats
        if (m0 == 0) {
            predict_beat();
        }

        // if we are at a beat
        if (beat_counter == 0) {
            // indicate a beat should be output
            beat_due_in_frame = true;
            // recalculate the tempo
            calculate_tempo();

            return -estimated_tempo;
        }

        return estimated_tempo;
    }

    void
    BTrack::update_cumulative_score(double df_sample)
    {
        auto start = df_buffer.size() - (int) round(2.0 * beat_period);
        auto end = df_buffer.size() - (int) round(beat_period / 2.0);
        auto win_size = end - start + 1;

        double w1[win_size];
        double v = -2.0 * beat_period;
        double cum_score;

        for (int i = 0; i < win_size; i++) {
            w1[i] = std::exp((-1 * std::pow(TIGHTNESS * std::log(-v / beat_period), 2)) / 2);
            v += 1.0;
        }

        double max = 0.0;
        int n = 0;

        for (auto i = start; i <= end; i++) {
            cum_score = cumulative_score[i] * w1[n];
            if (cum_score > max) {
                max = cum_score;
            }
            n++;
        }

        cum_score = ((1.0 - ALPHA) * df_sample + (ALPHA * max));
        cumulative_score.push_back(cum_score);
    }

    void
    BTrack::resample_DF()
    {
        auto src_ratio = (double) resampled_DF.size() / (double) df_buffer.size();

        auto df_buffer_size = df_buffer.size();

        if (df_buffer.size() == resampled_DF.size()) {
            /*
            std::copy(
                    df_buffer.begin(),
                    df_buffer.end(),
                    resampled_DF.begin()
            );
            */

            for (auto i = 0; i < df_buffer_size; i++) {
                resampled_DF[i] = (float) df_buffer[i];
            }
        }
        // src_stuff
    }

    void
    BTrack::calculate_tempo()
    {
        resample_DF();
        math_utilities::adaptive_threshold(resampled_DF);
        acf.perform_ACF(resampled_DF);
        //comb_filter(resampled_DF);
        //math_utilities::adaptive_threshold(resampled_DF);

        int t_index;
        int t_index2;

        for (auto i = 0; i < observation_vector.size(); i++) {
            t_index = (int) round(tempo_to_lag_factor / ((double) i / 8. + MIN_TEMPO));
            t_index2 = (int) round(tempo_to_lag_factor / ((double) i / 4. + MAX_TEMPO));

            observation_vector[i]
                    = resampled_DF[t_index - 1]
                      + resampled_DF[t_index2 - 1];
        }

        double max_val;
        int max_ind;
        double cur_val;

        for (auto j = 0; j < DELTA_SIZE; j++) {
            max_val = -1.;
            for (auto i = 0; i < DELTA_SIZE; i++) {
                cur_val = previous_delta[i] * transition_matrix[i][j];
                if (cur_val > max_val) {
                    max_val = cur_val;
                }
            }
            delta[j] = max_val * observation_vector[j];
        }

        math_utilities::normalize(delta);

        max_ind = -1;
        max_val = -1.;

        for (auto j = 0; j < DELTA_SIZE; j++) {
            if (delta[j] > max_val) {
                max_val = delta[j];
                max_ind = j;
            }
            previous_delta[j] = delta[j];
        }

        beat_period =
                (60. * sample_rate)
                / (max_ind / 8.0 + MIN_TEMPO)
                / (double) step_size
                / 1.0022;


        if (beat_period > 0.) {
            estimated_tempo = (60. * sample_rate) / (step_size * beat_period);
        }
    }

    void
    BTrack::predict_beat()
    {
        auto buffer_size = df_buffer.size();
        auto window_size = (unsigned int) beat_period;
        std::vector<double> future_cumulative_score(buffer_size + window_size);
        std::vector<double> w2(window_size);

        std::copy(
                cumulative_score.begin(),
                cumulative_score.end(),
                future_cumulative_score.begin()
        );

        double v = 1.0;
        for (int i = 0; i < window_size; i++) {
            w2[i] = std::exp(
                    (-1.0 * std::pow((v - (beat_period / 2.0)), 2.0))
                    / (2.0 * std::pow((beat_period / 2.0), 2.0))
            );
            v++;
        }

        v = -2.0 * beat_period;
        auto start = buffer_size - (int) round(2.0 * beat_period);
        auto end = buffer_size - (int) round(beat_period / 2.0);
        auto past_window_size = end - start + 1;

        std::vector<double> w1(past_window_size);

        for (auto i = 0; i < past_window_size; i++) {
            w1[i] = std::exp((-1.0 * std::pow(TIGHTNESS * std::log(-v / beat_period), 2.0)) / 2.0);
            v += 1.0;
        }

        double max;
        int n;
        double cum_score;
        for (auto i = buffer_size; i < (buffer_size + window_size); i++) {
            start = i - (int) round(2.0 * beat_period);
            end = i - (int) round(beat_period / 2.0);

            max = 0.0;
            n = 0;

            for (auto k = start; k <= end; k++) {
                cum_score = future_cumulative_score[k] * w1[n];
                if (cum_score > max) {
                    max = cum_score;
                }
                n++;
            }

            future_cumulative_score[i] = max;
        }

        max = 0.0;
        n = 0;

        for (auto i = buffer_size; i < (buffer_size + window_size); i++) {
            cum_score = future_cumulative_score[i] * w2[n];
            if (cum_score > max) {
                max = cum_score;
                beat_counter = n;
            }

            n++;
        }

        m0 = beat_counter + (int) round(beat_period / 2.0);
    }

    void
    BTrack::comb_filter(const std::vector<float>& acf)
    {
        comb_filter_bank_output.fill(0);

        auto numelem = 4; // number of comb elements
        auto max_beat_period = comb_filter_bank_output.size();
        for (auto i = 2; i < max_beat_period; i++) {
            for (auto a = 1; a <= numelem; a++) {
                for (auto b = 1 - a; b <= a - 1; b++) {
                    comb_filter_bank_output[i - 1]
                            = comb_filter_bank_output[i - 1]
                            + (acf[a * i + b - 1]
                               * weighting_vector[i - 1]) / (2 * a - 1);
                }
            }
        }
    }
}

