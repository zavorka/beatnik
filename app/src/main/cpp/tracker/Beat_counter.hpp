#pragma once

#include <vector>
#include <deque>
#include <array>
#include <complex>
#include "../external/RealTime/RealTime.hpp"

#pragma GCC visibility push(default)

namespace reBass {
    class CSD_detection_function;

    struct Beat {
        RealTime timestamp;
        float bpm;
    };

    class Beat_counter {
    public:
        Beat_counter(float inputSampleRate);

        virtual ~Beat_counter();

        void reset();

        unsigned int getStepSize() const;
        unsigned int getBlockSize() const;

        double process(
                const std::vector<std::complex<float>>& inputBuffer,
                RealTime timestamp
        );

        std::vector<Beat> getBeats();
        float getBpm();

    protected:
        CSD_detection_function *mDetectionFunction;
        std::deque<double> mDfOutput;
        RealTime mOrigin;

        // MEPD new protected parameters to allow the user to control these advanced parameters of the beat tracker
        double m_alpha;
        double m_tightness;
        float mInputSampleRate;

        static float mStepSecs;

        unsigned int getHalfLength() const;
    };

}

#pragma GCC visibility pop