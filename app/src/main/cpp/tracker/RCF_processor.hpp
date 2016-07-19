//
// Created by Roman Beránek on 7/17/16.
//

#pragma once

#include <array>
#include <cmath>

#include "Balanced_ACF.hpp"
#include "math_utilities.hpp"

namespace reBass
{
    template <size_t WindowLength>
    class RCF_processor
    {
    public:
        static_assert(
                WindowLength % 4 == 0,
                "WindowLength must be a multiple of 4."
        );

        static constexpr std::size_t OutputLength = WindowLength / 4;

        RCF_processor(double ray_param)
        {
            for (int n = 0; n < weighting_vector.size(); n++) {
                weighting_vector[n] =
                        n / (ray_param * ray_param)
                        * std::exp(
                                -1. * n * n
                                / (2. * std::pow(ray_param, 2.0))
                        );
            }
        };

        template <typename InputIterator>
        std::array<double, OutputLength>
        get_rcf(
                InputIterator input
        ) {
            std::copy(
                    input,
                    input + WindowLength,
                    df_frame.begin()
            );
            math_utilities::adaptive_threshold(df_frame);
            acf.perform_ACF(df_frame);

            std::array<double, OutputLength> out;
            math_utilities::comb_filter(
                    df_frame,
                    weighting_vector,
                    out
            );
            math_utilities::adaptive_threshold(out);
            math_utilities::normalize(out);

            return out;
        };
    private:
        std::array<float, WindowLength> df_frame;
        Balanced_ACF<WindowLength> acf;
        std::array<double, OutputLength> weighting_vector;
    };
}

