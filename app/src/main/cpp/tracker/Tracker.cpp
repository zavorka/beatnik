#include <algorithm>
#include <cmath>
#include <numeric>

#include "math_utilities.hpp"
#include "Tracker.hpp"

#define   EPS 0.0000008 // just some arbitrary small number

namespace reBass
{
    Tracker::Tracker(size_t increment, unsigned int sample_rate)
            : mIncrement(increment),
              sample_rate(sample_rate)
    {
    }

    void Tracker::calculateBeatPeriod(const vector<double> &df, vector<double> &beatPeriod)
    {
        // split into 512 sample frames with a 128 hop size
        // calculate the acf,
        // then the rcf..
        // then call viterbi decoding with weight vector and transition matrix
        // and get best path

        unsigned int wvLen = 128;

        // note: 60*44100/512 is a magic number
        // this might (will?) break if a user specifies a different frame rate for the onset detection function
        double rayparam = (60 * 44100 / 512) / kInputTempo;

        // make rayleigh weighting curve
        vector<double> wv(wvLen);

        for (unsigned int i = 0; i < wv.size(); i++)
        {
            // standard rayleigh weighting over periodicities
            wv[i] = (double(i) / pow(rayparam, 2.)) * exp((-1. * pow(-double(i), 2.)) / (2. * pow(rayparam, 2.)));
        }

        // beat tracking frame size (roughly 6 seconds) and hop (1.5 seconds)
        unsigned int winLen = 512;
        unsigned int step = 128;

        // matrix to store output of comb filter bank, increment column of matrix at each frame
        vector<vector<double>> rcfMatrix;
        int colCounter = -1;

        // main loop for beat period calculation
        for (unsigned int i = 0; i + winLen < df.size(); i += step)
        {
            // get dfFrame
            vector<double> dfFrame(winLen);
            for (unsigned int k = 0; k < winLen; k++)
            {
                dfFrame[k] = df[i+k];
            }
            // get rcf vector for current frame
            vector<double> rcf(wvLen);
            getRcf(dfFrame, wv, rcf);

            rcfMatrix.push_back(vector<double>()); // adds a new column
            colCounter++;
            for (unsigned int j = 0; j < rcf.size(); j++) {
                rcfMatrix[colCounter].push_back(rcf[j]);
            }
        }

        // now call viterbi decoding function
        viterbiDecode(rcfMatrix, wv, beatPeriod);
    }


    void Tracker::getRcf(const vector<double> &dfFrameIn, const vector<double> &wv, vector<double> &rcf)
    {
        // calculate autocorrelation function
        // then rcf
        // just hard code for now... don't really need separate functions to do this

        vector<double> dfFrame(dfFrameIn);
        math_utilities::adaptive_threshold(dfFrame);
        vector<double> acf(dfFrame.size());

        for (unsigned int lag = 0; lag < dfFrame.size(); lag++)
        {
            double sum = 0.;
            double tmp;

            for (unsigned int n=0; n < (dfFrame.size() - lag); n++) {
                tmp = dfFrame[n] * dfFrame[n+lag];
                sum += tmp;
            }
            acf[lag] = sum / (dfFrame.size() - lag);
        }

        // now apply comb filtering
        int numelem = 4;

        for (unsigned int i = 2; i < rcf.size(); i++) // max beat period
        {
            for (int a = 1; a <= numelem; a++) // number of comb elements
            {
                for (int b = 1 - a; b <= a - 1; b++) // general state using normalisation of comb elements
                {
                    rcf[i - 1] += (acf[(a * i + b) - 1] * wv[i - 1]) / (2. * a - 1.);	// calculate value for comb filter row
                }
            }
        }

        // apply adaptive threshold to rcf
        math_utilities::adaptive_threshold(rcf);

        double rcfsum = 0.;
        for (unsigned int i = 0; i < rcf.size(); i++) {
            rcf[i] += EPS ;
            rcfsum += rcf[i];
        }

        // normalise rcf to sum to unity
        for (unsigned int i=0; i < rcf.size(); i++) {
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

        // make transition matrix
        vector<vector<double>> tmat;
        for (unsigned int i = 0; i < wv.size(); i++) {
            tmat.push_back (vector<double>()); // adds a new column
            for (unsigned int j = 0; j < wv.size(); j++) {
                tmat[i].push_back(0.); // fill with zeros initially
            }
        }

        // variance of Gaussians in transition matrix
        // formed of Gaussians on diagonal - implies slow tempo change
        double sigma = 8.;
        // don't want really short beat periods, or really long ones
        for (unsigned int i = 20; i < wv.size() - 20; i++) {
            for (unsigned int j = 20; j < wv.size() - 20; j++) {
                double mu = double(i);
                tmat[i][j] = exp((-1. * pow((j - mu), 2.)) / (2. * pow(sigma, 2.)));
            }
        }

        // parameters for Viterbi decoding
        vector<vector<double>> delta;
        vector<vector<int>> psi;
        for (unsigned int i = 0;i <rcfmat.size(); i++)
        {
            delta.push_back(vector<double>());
            psi.push_back(vector<int>());
            for (unsigned int j = 0; j < rcfmat[i].size(); j++) {
                delta[i].push_back(0.); // fill with zeros initially
                psi[i].push_back(0); // fill with zeros initially
            }
        }


        unsigned long T = delta.size();

        if (T < 2) {
            return; // can't do anything at all meaningful
        }

        unsigned long Q = delta[0].size();

        // initialize first column of delta
        for (unsigned long j = 0; j < Q; j++)
        {
            delta[0][j] = wv[j] * rcfmat[0][j];
            psi[0][j] = 0;
        }

        double deltaSum = std::accumulate(delta[0].begin(), delta[0].end(), 0.0);

        for (unsigned long i = 0; i < Q; i++) {
            delta[0][i] /= (deltaSum + EPS);
        }


        for (unsigned long t = 1; t < T; t++)
        {
            vector<double> tmpVec(Q);

            for (unsigned long j = 0; j < Q; j++)
            {
                for (unsigned long i = 0; i < Q; i++) {
                    tmpVec[i] = delta[t-1][i] * tmat[j][i];
                }

                delta[t][j] = getMaxValue(tmpVec);
                psi[t][j] = getMaxIndex(tmpVec);
                delta[t][j] *= rcfmat[t][j];
            }

            // normalise current delta column
            deltaSum = std::accumulate(delta[t].begin(), delta[t].begin() + Q, 0.0);

            for (unsigned int i=0; i<Q; i++) {
                delta[t][i] /= (deltaSum + EPS);
            }
        }

        vector<int> bestpath(T);
        vector<double> tmpVec(Q);
        for (unsigned int i = 0; i < Q; i++) {
            tmpVec[i] = delta[T-1][i];
        }

        // find starting point - best beat period for "last" frame
        bestpath[T-1] = getMaxIndex(tmpVec);

        // backtrace through index of maximum values in psi
        for (unsigned long t = T - 2; t > 0; t--) {
            bestpath[t] = psi[t + 1][bestpath[t + 1]];
        }

        // weird but necessary hack -- couldn't get above loop to terminate at t >= 0
        bestpath[0] = psi[1][bestpath[1]];

        unsigned int lastind = 0;
        for (unsigned int i = 0; i < T; i++)
        {
            unsigned int step = 128;
            for (unsigned int j = 0; j < step; j++)
            {
                lastind = i * step + j;
                beatPeriod[lastind] = bestpath[i];
            }
        }

        for (unsigned int i = lastind; i < beatPeriod.size(); i++) {
            beatPeriod[i] = beatPeriod[lastind];
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

    void Tracker::calculateBeats(
            const vector<double> &df,
            const vector<double> &beatPeriod,
            vector<double> &beats
    ) {
        if (df.empty() || beatPeriod.empty()) return;

        vector<double> cumscore(df.size()); // store cumulative score
        vector<int> backlink(df.size()); // backlink (stores best beat locations at each time instant)
        vector<double> localscore(df.size()); // localscore, for now this is the same as the detection function

        for (unsigned int i = 0; i < df.size(); i++)
        {
            localscore[i] = df[i];
            backlink[i] = -1;
        }


        // main loop
        for (unsigned int i = 0; i < localscore.size(); i++)
        {
            int prangeMin = (int) (-2 * beatPeriod[i]);
            int prangeMax = (int) round(-0.5 * beatPeriod[i]);

            // transition range
            vector<double> txwt (prangeMax - prangeMin + 1);
            vector<double> scorecands (txwt.size());

            for (unsigned int j = 0; j < txwt.size(); j++)
            {
                double mu = static_cast<double> (beatPeriod[i]);
                txwt[j] = exp(-0.5 * pow(kTightness * log((round(2 * mu) - j) / mu), 2));

                int cscore_ind = i + prangeMin + j;
                if (cscore_ind >= 0) {
                    scorecands[j] = txwt[j] * cumscore[cscore_ind];
                }
            }

            double vv = getMaxValue(scorecands);
            int xx = getMaxIndex(scorecands);

            cumscore[i] = kAlpha * vv + (1. - kAlpha) * localscore[i];
            backlink[i] = i + prangeMin + xx;
        }

        // pick a strong point in cumscore vector
        vector<double> tmpVec;
        for (auto i = cumscore.size() - beatPeriod[beatPeriod.size()-1] ; i < cumscore.size(); i++) {
            tmpVec.push_back(cumscore[i]);
        }

        auto startpoint = getMaxIndex(tmpVec) + cumscore.size() - beatPeriod[beatPeriod.size()-1] ;

        // can happen if no results obtained earlier (e.g. input too short)
        if (startpoint >= (int)backlink.size()) startpoint = (int)(backlink.size()-1);

        vector<int> ibeats;
        ibeats.push_back((int) startpoint);
        while (backlink[ibeats.back()] > 0)
        {
            int b = ibeats.back();
            if (backlink[b] == b) break; // shouldn't happen... haha
            ibeats.push_back(backlink[b]);
        }

        for (unsigned int i = 0; i < ibeats.size(); i++) {
            beats.push_back((double) ibeats[ibeats.size() - i - 1]);
        }
    }
}
