//
// Created by Roman Beránek on 7/14/16.
//

#pragma once

#include <cstddef>
#include <array>
#include <vector>
#include <boost/circular_buffer.hpp>

#include "Balanced_ACF.hpp"
#include "RCF_processor.hpp"
#include "Viterbi_decoder.hpp"

namespace reBass {
    class BTrack {
    public:
        BTrack(
                unsigned int sample_rate,
                unsigned int step_size
        );
        float process_DF_sample(float sample);
        float process_DF_samples(const std::vector<float> &samples);
    private:
        static constexpr double TIGHTNESS = 5.0;
        static constexpr double ALPHA = 0.9;
        static constexpr float MIN_TEMPO = 90.0f;
        static constexpr float MAX_TEMPO = MIN_TEMPO * 2.f;
        static constexpr float DEFAULT_TEMPO = (MAX_TEMPO + MIN_TEMPO) / 2.f;
        static constexpr unsigned int DF_LENGTH = 2048;
        static constexpr unsigned int DF_STEP = 128;
        static constexpr unsigned int DF_WINDOW = 512;
        static constexpr unsigned int RCF_ROWS = DF_LENGTH / DF_STEP;
        static constexpr unsigned int RCF_COLUMNS = 128;

        const unsigned int sample_rate;
        const unsigned int step_size;
        const double tempo_to_lag_factor;
        const double ray_param;

        std::size_t counter;
        std::size_t countdown;

        double beat_period;

        bool should_calculate_periods;
        bool should_calculate_tempo;


        boost::circular_buffer<float> df_buffer;
        //boost::circular_buffer<double> cumulative_score;
        std::array<double, DF_LENGTH> cumulative_score;
        std::array<int, DF_LENGTH> backlink;

        RCF_processor<DF_WINDOW> rcf_processor;
        Viterbi_decoder<RCF_ROWS, RCF_COLUMNS> decoder;

        boost::circular_buffer<std::array<double, RCF_COLUMNS>> rcf_buffer;
        std::array<std::array<double, RCF_COLUMNS>, RCF_ROWS> rcf_matrix;

        std::vector<int> beats;
        std::array<std::size_t, RCF_ROWS> periods;

        struct Period_constants {
            const int min_range;
            const int max_range;
            const int range_length;

            std::vector<double> txwt;

            Period_constants(int period)
                    : min_range(-2 * period),
                      max_range((int) round(-0.5 * period)),
                      range_length(max_range - min_range + 1),
                      txwt(range_length)
            {
                auto mu = static_cast<double>(period);
                for (auto i = 0; i < range_length; i++) {
                    txwt[i] = std::exp(-0.5 * std::pow(TIGHTNESS * std::log(2. - (i / mu)), 2));
                }
            }
        };

        std::vector<Period_constants> period_constants;

        float calculate_tempo();
        void calculate_rcf();
        void calculate_periods();
    };
}
