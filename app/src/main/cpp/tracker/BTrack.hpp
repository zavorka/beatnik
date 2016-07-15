//
// Created by Roman Beránek on 7/14/16.
//

#pragma once

#include <cstddef>
#include <array>
#include <vector>
#include <boost/circular_buffer.hpp>

#include "../beatnik/Balanced_ACF.hpp"

namespace reBass {
    class BTrack {
    public:
        BTrack(
                unsigned int sample_rate,
                unsigned int step_size
        );
        double process_DF_sample(double sample);
    private:
        static constexpr double RAY_PARAM = 963.0;
        static constexpr double TIGHTNESS = 3.0;
        static constexpr double ALPHA = 0.9;
        static constexpr double DEFAULT_TEMPO = 150.0;
        static constexpr double MAX_TEMPO = 240.0;
        static constexpr double MIN_TEMPO = 120.0;
        static constexpr double M_SIG = 961.0 / 8.0;
        static constexpr unsigned int RESAMPLED_DF_LENGTH = 2048;
        static constexpr unsigned int ACF_FFT_LENGTH = RESAMPLED_DF_LENGTH * 2;
        static constexpr unsigned int COMB_FILTER_BANK_OUTPUT_SIZE = 128;
        static constexpr unsigned int DELTA_SIZE = 961;

        const unsigned int sample_rate;
        const unsigned int step_size;
        const double tempo_to_lag_factor;

        bool beat_due_in_frame;

        int beat_counter;
        int m0;

        double beat_period;
        double estimated_tempo;

        boost::circular_buffer<double> df_buffer;
        boost::circular_buffer<double> cumulative_score;

        std::vector<float> resampled_DF;
        Balanced_ACF acf;

        std::array<double, 128> comb_filter_bank_output;
        std::array<double, 128> weighting_vector;

        std::array<double, 961> observation_vector;
        std::array<std::array<double, 961>, 961> transition_matrix;
        std::vector<double> delta;
        std::array<double, 961> previous_delta;

        void predict_beat();
        void update_cumulative_score(double df_sample);
        void resample_DF();

        void calculate_tempo();

        void comb_filter(const std::vector<float> &acf);
    };
}
