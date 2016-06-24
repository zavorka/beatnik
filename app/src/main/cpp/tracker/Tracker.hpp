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
                size_t dfIncrement,
                unsigned int sample_rate
        );

        // Returned beat periods are given in df increment units;
        // inputtempo and tempi in bpm
        void calculateBeatPeriod(
                const vector<double> &df,
                vector<double> &beatPeriod)
        ;

        // Returned beat positions are given in df increment units
        void calculateBeats(
                const vector<double> &df,
                const vector<double> &beatPeriod,
                vector<double> &beats
        );

    private:
        size_t mIncrement;
        unsigned int sample_rate;

        void getRcf(
                const vector<double> &dfFrameIn,
                const vector<double> &wv,
                vector<double> &rcf
        );
        void viterbiDecode(
                const vector<vector<double>> &rcfmat,
                const vector<double> &wv,
                vector<double> &beatPeriod
        );
        double getMaxValue(const vector<double> &df);
        int getMaxIndex(const vector<double> &df);

        const float kInputTempo = 140.f;
        const float kTightness = 4.f;
        const float kAlpha = .9f;
    };
}

#pragma GCC visibility pop
