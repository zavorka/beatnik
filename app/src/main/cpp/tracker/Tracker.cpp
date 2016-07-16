#include <algorithm>
#include <cmath>
#include <numeric>

#include "math_utilities.hpp"
#include "Tracker.hpp"
#include "../log.h"

#define   EPS 0.0000008 // just some arbitrary small number

using std::exp;
using std::pow;
using std::vector;

namespace reBass
{
    Tracker::Tracker(
            unsigned int step_size,
            unsigned int sample_rate,
            unsigned int df_length
    )
            : step_size(step_size),
              sample_rate(sample_rate),
              df_length(df_length),
              step_rate((float) sample_rate / (float) step_size),
              ray_param((60. / INPUT_TEMPO) * step_rate),
              weight_vector(RCF_LENGTH),
              rcf_matrix ((df_length - DF_WINDOW) / DF_STEP,
                          vector<double>(RCF_LENGTH)),
              df_frame(DF_WINDOW),
              acf_frame(DF_WINDOW),
              transition_matrix(RCF_LENGTH, vector<double>(RCF_LENGTH)),
              delta(RCF_LENGTH, vector<double>(RCF_LENGTH)),
              psi(RCF_LENGTH, vector<int>(RCF_LENGTH)),
              best_path(RCF_LENGTH),
              cumulative_score(df_length),
              backlink(df_length),
              local_score(df_length)
    {
        // calculate the vector for Rayleigh wighting over periodicities
        double ray_param_sqr = pow(ray_param, 2);
        for (auto i = 0.; i < RCF_LENGTH; i++) {
            weight_vector.at((unsigned long) i)
                    = i / ray_param_sqr
                      * exp(-1. * pow(i, 2) / (2. * ray_param_sqr));
        }

        // don't want really short beat periods, or really long ones
        for (auto i = 20; i < RCF_LENGTH - 20; i++) {
            for (auto j = 20; j < RCF_LENGTH - 20; j++) {
                transition_matrix[i][j] =
                        exp((-1. * (j - i) * (j - i)) / (2. * pow(SIGMA, 2.)));
            }
        }
        LOGI("Tracker Passed init");
    }

    void
    Tracker::calculateBeatPeriod(
            const vector<double> &df,
            vector<double> &beat_periods
    ) {
        // main loop for beat period calculation
        for (unsigned int i = 0; i + DF_WINDOW < df_length; i += DF_STEP)
        {
            // get dfFrame
            std::copy(df.cbegin() + i, df.cbegin() + i + DF_WINDOW, df_frame.begin());
            // get rcf vector for current frame
            getRcf(rcf_matrix[i / DF_STEP]);
        }

        // now call viterbi decoding function
        viterbiDecode(rcf_matrix, weight_vector, beat_periods);
    }


    void
    Tracker::getRcf(
            vector<double> &rcf
    ) {
        // calculate auto-correlation function
        // then rcf
        // just hard code for now... don't really need separate functions to do this

        math_utilities::adaptive_threshold(df_frame);

        for (unsigned int lag = 0; lag < DF_WINDOW; lag++) {
            double sum = 0.;
            for (unsigned int n = 0; n < (DF_WINDOW - lag); n++) {
                sum += df_frame[n] * df_frame[n + lag];
            }
            acf_frame[lag] = sum / (DF_WINDOW - lag);
        }

        // now apply comb filtering
        for (auto i = 2; i < RCF_LENGTH; i++) { // max beat period
            for (auto a = 1; a <= COMB_SIZE; a++) {
                // general state using normalisation of comb elements
                for (auto b = 1 - a; b <= a - 1; b++) {
                    // calculate value for comb filter row
                    rcf[i - 1] += (acf_frame[(a * i + b) - 1]
                                   * weight_vector[i - 1]) / (2. * a - 1.);
                }
            }
        }

        // apply adaptive threshold to rcf
        math_utilities::adaptive_threshold(rcf);

        double rcfsum = 0.;
        for (auto i = 0; i < RCF_LENGTH; i++) {
            rcf[i] += EPS ;
            rcfsum += rcf[i];
        }

        // normalise rcf to sum to unity
        for (auto i = 0; i < RCF_LENGTH; i++) {
            rcf[i] /= (rcfsum + EPS);
        }
    }

    void Tracker::viterbiDecode(
            const vector<vector<double>> &rcfmat,
            const vector<double> &wv,
            vector<double> &beatPeriod
    ) {
        // following Kevin Murphy's Viterbi decoding to get best path of
        // beat periods through rfcmat


        for (auto i = 0;i < RCF_LENGTH; i++) {
            for (auto j = 0; j < RCF_LENGTH; j++) {
                delta[i][j] = 0.;
                psi[i][j] = 0;
            }
        }


        auto T = rcfmat.size();
        auto Q = RCF_LENGTH;

        // initialize first column of delta
        for (auto j = 0; j < Q; j++)
        {
            delta[0][j] = weight_vector[j] * rcfmat[0][j];
            psi[0][j] = 0;
        }

        auto deltaSum = std::accumulate(delta[0].cbegin(), delta[0].cend(), 0.0);

        for (auto i = 0; i < Q; i++) {
            delta[0][i] /= (deltaSum + EPS);
        }


        for (unsigned long t = 1; t < T; t++) {
            for (auto j = 0; j < Q; j++) {
                auto max_value = delta[t-1][0] * transition_matrix[j][0];
                auto max_index = 0;
                for (auto i = 0; i < Q; i++) {
                    auto value = delta[t-1][i] * transition_matrix[j][i];
                    if (value > max_value) {
                        max_value = value;
                        max_index = i;
                    }
                }

                delta[t][j] = max_value;
                psi[t][j] = max_index;
                delta[t][j] *= rcfmat[t][j];
            }

            // normalise current delta column
            deltaSum = std::accumulate(delta[t].begin(), delta[t].begin() + Q, 0.0);
            for (auto i = 0; i < Q; i++) {
                delta[t][i] /= (deltaSum + EPS);
            }
        }

        // find starting point - best beat period for "last" frame
        auto max_value = delta[T - 1][0];
        auto max_index = 0;
        for (auto i = 0; i < Q; i++) {
            if (delta[T - 1][i] > max_value) {
                max_value = delta[T - 1][i];
                max_index = i;
            }
        }
        best_path[T - 1] = max_index;

        // backtrace through index of maximum values in psi
        for (auto t = T - 2; t > 0; t--) {
            best_path[t] = psi[t + 1][best_path[t + 1]];
        }

        // weird but necessary hack -- couldn't get above loop to terminate at t >= 0
        best_path[0] = psi[1][best_path[1]];
        unsigned int last_index = 0;
        for (auto i = 0; i < T; i++)
        {
            for (unsigned int j = 0; j < DF_STEP; j++)
            {
                last_index = i * DF_STEP + j;
                beatPeriod[last_index] = best_path[i];
            }
        }

        for (auto i = last_index; i < beatPeriod.size(); i++) {
            beatPeriod[i] = beatPeriod[last_index];
        }
    }

    double Tracker::getMaxValue(const vector<double> &df)
    {
        return *max_element(df.begin(), df.end());
    }

    int Tracker::getMaxIndex(const vector<double> &df)
    {
        return (int) (std::max_element(df.begin(), df.end()) - df.begin());
    }

    vector<double>
    Tracker::calculateBeats(
            const vector<double> &df,
            const vector<double> &beatPeriod
    ) {
        vector<double> beats;

        if (df.size() != df_length) return beats;

        std::copy(df.cbegin(), df.cend(), local_score.begin());
        for (auto i = 0; i < df_length; i++) {
            backlink[i] = -1;
        }

        // main loop
        for (auto i = 0; i < df_length; i++)
        {
            int prangeMin = (int) (-2 * beatPeriod[i]);
            int prangeMax = (int) round(-0.5 * beatPeriod[i]);

            // transition range
            vector<double> scorecands (prangeMax - prangeMin + 1);

            for (unsigned int j = 0; j < scorecands.size(); j++)
            {
                double mu = static_cast<double> (beatPeriod[i]);
                double txwt = exp(-0.5 * pow(TIGHTNESS * log((round(2 * mu) - j) / mu), 2));

                int cscore_ind = i + prangeMin + j;
                if (cscore_ind >= 0) {
                    scorecands[j] = txwt * cumulative_score[cscore_ind];
                }
            }

            double vv = getMaxValue(scorecands);
            int xx = getMaxIndex(scorecands);

            cumulative_score[i] = ALPHA * vv + (1. - ALPHA) * local_score[i];
            backlink[i] = i + prangeMin + xx;
        }

        // pick a strong point in cumscore vector
        auto period = (unsigned int) beatPeriod[beatPeriod.size()-1];
        auto max_score = cumulative_score[df_length - period];
        auto max_index = 0;
        for (auto i = df_length -  period; i < df_length; i++) {
            if(cumulative_score[i] > max_score) {
                max_score = cumulative_score[i];
                max_index = i;
            }
        }

        vector<int> ibeats;
        ibeats.push_back(max_index);
        while (backlink[ibeats.back()] > 0)
        {
            int b = ibeats.back();
            if (backlink[b] == b) break; // shouldn't happen... haha
            ibeats.push_back(backlink[b]);
        }

        for (auto i = 0; i < ibeats.size(); i++) {
            beats.push_back((double) ibeats[ibeats.size() - i - 1]);
        }

        return beats;
    }
}
