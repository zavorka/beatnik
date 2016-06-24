#include <numeric>
#include <cmath>
#include "math_utilities.hpp"

using std::vector;

namespace reBass
{
    double math_utilities::princarg(double ang)
    {
        return fmod(ang + PI, -2 * PI) + PI;
    }

    double math_utilities::mean(const vector<double> &src, int start, int count)
    {
        return std::accumulate(src.begin() + start, src.begin() + start + count, 0.0) / count;
    }

    void math_utilities::adaptive_threshold(vector<double> &data)
    {
        int sz = int(data.size());
        if (sz == 0) return;

        vector<double> smoothed((unsigned long) sz);

        int p_pre = 8;
        int p_post = 7;

        for (int i = 0; i < sz; ++i) {

            int first = std::max(0,      i - p_pre);
            int last  = std::min(sz - 1, i + p_post);

            smoothed[i] = mean(data, first, last - first + 1);
        }

        for (int i = 0; i < sz; i++) {
            data[i] -= smoothed[i];
            if (data[i] < 0.0) data[i] = 0.0;
        }
    }
}
