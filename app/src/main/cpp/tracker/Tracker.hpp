#pragma once
#include <vector>

#pragma GCC visibility push(hidden)

using namespace std;

namespace reBass
{
    class Tracker
    {
    public:
        /**
         * Construct a tempo tracker that will operate on beat detection
         * function data calculated from audio at the given sample rate
         * with the given frame increment.
         *
         * Currently the sample rate and increment are used only for the
         * conversion from beat frame location to bpm in the tempo array.
         */
        Tracker(
                unsigned int step_size,
                unsigned int sample_rate,
                unsigned int df_length
        );

        // Returned beat periods are given in df increment units;
        // inputtempo and tempi in bpm
        void calculateBeatPeriod(
                const vector<double> &df,
                vector<double> &beat_periods)
        ;

        // Returned beat positions are given in df increment units
        vector<double> calculateBeats(
                const vector<double> &df,
                const vector<double> &beatPeriod
        );

    private:
        static constexpr float INPUT_TEMPO = 135.f;
        static constexpr float TIGHTNESS = 4.f;
        static constexpr float ALPHA = .9f;
        // variance of Gaussians in transition matrix
        // formed of Gaussians on diagonal - implies slow tempo change
        static constexpr double SIGMA = 8.;

        static constexpr unsigned int RCF_LENGTH = 128;
        static constexpr unsigned int COMB_SIZE = 4;
        static constexpr unsigned int DF_WINDOW = 512;
        static constexpr unsigned int DF_STEP = 128;

        const unsigned int step_size;
        const unsigned int sample_rate;
        const unsigned int df_length;

        const float step_rate;
        const double ray_param;

        std::vector<double> weight_vector;
        // matrix to store output of comb filter bank
        std::vector<std::vector<double>> rcf_matrix;
        std::vector<double> df_frame;
        std::vector<double> acf_frame;
        std::vector<std::vector<double>> transition_matrix;
        // parameters for Viterbi decoding
        std::vector<std::vector<double>> delta;
        std::vector<std::vector<int>> psi;
        std::vector<int> best_path;

        std::vector<double> cumulative_score;
        // best beat locations at each time instant
        std::vector<int> backlink;
        std::vector<double> local_score;

        void getRcf(
                vector<double> &rcf
        );
        void viterbiDecode(
                const vector<vector<double>> &rcfmat,
                const vector<double> &wv,
                vector<double> &beatPeriod
        );
        double getMaxValue(const vector<double> &df);
        int getMaxIndex(const vector<double> &df);
    };
}

#pragma GCC visibility pop
