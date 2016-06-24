#include "Beat_counter.hpp"
#include "CSD_detection_function.hpp"
#include "Tracker.hpp"

using std::vector;
using std::complex;

namespace reBass
{
    float Beat_counter::mStepSecs = 0.01161f;

    Beat_counter::Beat_counter(float inputSampleRate) : mInputSampleRate(inputSampleRate)
    {
        mDetectionFunction = new CSD_detection_function(getHalfLength(), getStepSize());
    }

    Beat_counter::~Beat_counter() {
        delete mDetectionFunction;
    }


    void Beat_counter::reset()
    {
        delete mDetectionFunction;
        mDetectionFunction = new CSD_detection_function(getBlockSize(), getStepSize());
        mDfOutput.clear();
        mOrigin = RealTime::zeroTime;
    }

    unsigned int Beat_counter::getStepSize() const {
        return (unsigned int) (mInputSampleRate * mStepSecs + 0.0001);
    }

    unsigned int Beat_counter::getBlockSize() const {
        return getStepSize() * 2;
    }

    unsigned int Beat_counter::getHalfLength() const {
        return getBlockSize() / 2 + 1;
    }


    double Beat_counter::process(
            const vector<complex<float>> &inputBuffer,
            RealTime timestamp
    ) {
        double output = mDetectionFunction->processFrequencyDomain(inputBuffer);

        if (mDfOutput.empty()) mOrigin = timestamp;
        if (mDfOutput.size() >= 2048) {
            mDfOutput.erase(mDfOutput.begin());
        }

        mDfOutput.push_back(output);

        return output;
    }

    vector<Beat> Beat_counter::getBeats()
    {
        size_t nonZeroCount = mDfOutput.size();
        while (nonZeroCount > 0) {
            if (mDfOutput[nonZeroCount - 1] > 0.0) {
                break;
            }
            --nonZeroCount;
        }

        if (nonZeroCount <= 2) {
            return vector<Beat>();
        }

        vector<double> df { mDfOutput.cbegin() + 2, mDfOutput.cbegin() + nonZeroCount };
        vector<double> beatPeriod(nonZeroCount - 2);

        Tracker tracker(mInputSampleRate, getStepSize());

        tracker.calculateBeatPeriod(df, beatPeriod);

        vector<double> beats;
        tracker.calculateBeats(df, beatPeriod, beats);

        vector<Beat> returnBeats;

        for (size_t i = 0; i < beats.size(); ++i)
        {
            size_t frame = (size_t) (beats[i] * getStepSize());
            Beat beat;
            beat.timestamp = mOrigin + RealTime::frame2RealTime((int)frame, (unsigned int)lrintf(mInputSampleRate));

            int frameIncrement = 0;

            if (i + 1 < beats.size())
            {
                frameIncrement = (int) ((beats[i + 1] - beats[i]) * getStepSize());
                if (frameIncrement > 0) {
                    float bpm = (60.0f * mInputSampleRate) / frameIncrement;
                    beat.bpm = int(bpm * 100.0f + 0.5f) / 100.0f;
                }
            }

            returnBeats.push_back(beat); // beats are output 0
        }

        return returnBeats;
    }

    float Beat_counter::getBpm() {
        auto bpms = getBeats();
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

        return bpm;
    }

}
