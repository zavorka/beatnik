#include "CSD_detection_function.hpp"
#include <numeric>

using std::complex;
using std::vector;

namespace reBass
{
    CSD_detection_function::CSD_detection_function(
            unsigned int frameLength,
            unsigned int stepSize
    )
        : frameLength(frameLength),
          stepSize(stepSize),
          magnitude(frameLength),
          previous_magnitude(frameLength),
          peak_magnitude(frameLength)
    {
    }

    double
    CSD_detection_function::process_magnitudes(
            const vector<float> &magnitudes
    ){
        std::copy(
            magnitudes.cbegin(),
            magnitudes.cend(),
            this->magnitude.begin()
        );
        //whiten();
        return broadband();
    }

    void
    CSD_detection_function::whiten()
    {
        for (unsigned int i = 0; i < frameLength; ++i) {
            double m = magnitude[i];
            if (m < peak_magnitude[i]) {
                m = m + (peak_magnitude[i] - m) * kWhitenRelaxCoeff;
            }
            if (m < kWhitenFloor) {
                m = kWhitenFloor;
            }
            peak_magnitude[i] = m;
            magnitude[i] /= m;
        }
    }

    double
    CSD_detection_function::broadband()
    {
        double val = 0;

        for (unsigned int i = 0; i < frameLength; i++) {
            double squared_magnitude = magnitude[i] * magnitude[i];
            if (previous_magnitude[i] > 0.0) {
                double diff = 10.0 * log10(squared_magnitude / previous_magnitude[i]);
                if (diff > DB_RISE) {
                    val = val + 1.0;
                }
            }
            previous_magnitude[i] = squared_magnitude;
        }

        return val;
    }
}
