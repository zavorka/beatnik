#include <numeric>
#include <cmath>
#include "math_utilities.hpp"

using std::vector;
using std::size_t;

namespace reBass
{
    double
    math_utilities::princarg(double ang)
    {
        return fmod(ang + PI, -2 * PI) + PI;
    }

    double
    math_utilities::mean(
            const vector<double> &src,
            size_t start,
            size_t count
    ) {
        return std::accumulate(
                src.begin() + start,
                src.begin() + start + count,
                0.0
            ) / count;
    }

    void
    math_utilities::adaptive_threshold(vector<double> &data)
    {
        unsigned long sz = data.size();
        if (sz == 0) return;

        vector<double> smoothed(sz);

        auto p_pre = 8;
        auto p_post = 7;

        for (unsigned long i = 0; i < sz; ++i) {
            size_t first = std::max((unsigned long) 0, i - p_pre);
            size_t last  = std::min(sz - 1,            i + p_post);

            smoothed[i] = mean(data, first, last - first + 1);
        }

        for (auto i = 0; i < sz; i++) {
            data[i] -= smoothed[i];
            if (data[i] < 0.0) data[i] = 0.0;
        }
    }
}
