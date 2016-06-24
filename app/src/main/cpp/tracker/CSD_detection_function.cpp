#include "CSD_detection_function.hpp"
#include <numeric>

using std::complex;
using std::vector;
using std::abs;
using std::arg;
using std::polar;

namespace reBass
{
    CSD_detection_function::CSD_detection_function(unsigned int frameLength, unsigned int stepSize)
        : frameLength(frameLength), stepSize(stepSize), data(frameLength)
    {
        for (size_t i = 0; i < frameLength; i++) {
            auto omega = i * PI * stepSize / (frameLength - 1);
            data[i].phase = -omega;
            data[i].unwrapped = -omega;
        }
    }

    double CSD_detection_function::processFrequencyDomain (const vector<complex<float>>& fft)
    {
        for (size_t i = 0; i < frameLength; i++) {
            data[i].magnitude = abs(fft[i]);
            double theta = arg(fft[i]);

            double omega = (2 * PI * stepSize * i) / stepSize;
            double expected = data[i].phase + omega;
            double error = math_utilities::princarg(theta - expected);

            data[i].unwrapped = data[i].unwrapped + omega + error;
            data[i].phase = theta;
        }

        whiten();

        return complexSD();
    }

    void CSD_detection_function::whiten()
    {
        for (unsigned int i = 0; i < frameLength; ++i) {
            double m = data[i].magnitude;
            if (m < data[i].magPeak) {
                m = m + (data[i].magPeak - m) * kWhitenRelaxCoeff;
            }
            if (m < kWhitenFloor) {
                m = kWhitenFloor;
            }
            data[i].magPeak = m;
            data[i].magnitude /= m;
        }
    }


    double CSD_detection_function::complexSD ()
    {
        double val = 0;

        for (unsigned int i = 0; i < frameLength; i++) {
    	    double phase = (data[i].magnitude - 2 * data[i].phaseHistory + data[i].phaseHistoryOld);
    	    double dev = math_utilities::princarg(phase);

            //std::complex<double> meas = mMagHistory[i] -
            //        (srcMagnitude[i] * exp(complex<double>(0, 1) * dev));
            //val += abs(meas);

            val += abs(data[i].magHistory - polar(data[i].magnitude, dev));

    	    data[i].phaseHistoryOld = data[i].phaseHistory;
    	    data[i].phaseHistory = data[i].phase;
    	    data[i].magHistory = data[i].magnitude;
        }

        return val;
    }
}
