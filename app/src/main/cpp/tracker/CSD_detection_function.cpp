#include "CSD_detection_function.hpp"
#include <numeric>

using std::complex;
using std::vector;

namespace reBass
{
    CSD_detection_function::CSD_detection_function(
            unsigned int frameLength
    )
        : frameLength(frameLength),
          magnitude(frameLength),
          previous_magnitude(frameLength),
          peak_magnitude(frameLength)
    {
    }

    float
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
            float m = magnitude[i];
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

    float
    CSD_detection_function::broadband()
    {
        float val = 0;

        for (unsigned int i = 0; i < frameLength; i++) {
            float squared_magnitude = magnitude[i] * magnitude[i];
            if (previous_magnitude[i] > 0.0f) {
                double diff = 10.0 * log10(squared_magnitude / previous_magnitude[i]);
                if (diff > DB_RISE) {
                    val = val + 1.0f;
                }
            }
            previous_magnitude[i] = squared_magnitude;
        }

        return val;
    }
}
