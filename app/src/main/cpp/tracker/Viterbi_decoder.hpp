//
// Created by Roman Beránek on 7/17/16.
//

#pragma once

#include <cstddef>
#include <array>
#include <vector>
#include <cmath>

#include "math_utilities.hpp"

using std::array;

namespace reBass
{
    template <std::size_t T, std::size_t Q>
    class Viterbi_decoder
    {
    public:
        //static_assert(T % 4 == 0, "");
        static_assert(Q % 4 == 0, "");

        Viterbi_decoder(float ray_param) noexcept
        {
            double m_sig = (double) Q / 8.;
            for (auto i = PADDING; i < Q - PADDING; i++) {
                for (auto j = PADDING; j < Q - PADDING; j++) {
                    transition_matrix[i][j] =
                            (float) std::exp(
                                    (-1. * (j - i) * (j - i))
                                    / (2. * m_sig * m_sig)
                            );
                }
            }

            for (int q = 0; q < Q; q++) {
                weighting_vector[q] =
                        q / (ray_param * ray_param)
                        * (float) std::exp(
                                -1. * q * q
                                / (2. * std::pow(ray_param, 2.0))
                        );
            }
        }

        const std::array<std::size_t, T>&
        decode(const array<array<float, Q>, T> &rcf_matrix) noexcept
        {
            for (auto q = 0; q < Q; q++) {
                delta[0][q] = weighting_vector[q] * rcf_matrix[0][q];
            }
            math_utilities::normalize(delta[0]);
            psi[0].fill(0);

            for (auto t = 1; t < T; t++) {
                for (auto j = 0; j < Q; j++) {
                    auto max_value = delta[t - 1][0] * transition_matrix[j][0];
                    auto max_index = 0;
                    for (auto i = 0; i < Q; i++) {
                        auto value = delta[t - 1][i] * transition_matrix[j][i];
                        if (value > max_value) {
                            max_value = value;
                            max_index = i;
                        }
                    }

                    delta[t][j] = max_value * rcf_matrix[t][j];
                    psi[t][j] = max_index;
                }
                math_utilities::normalize(delta[t]);
            }

            auto max_value = delta[T - 1][0];
            auto max_index = 0;

            for(auto q = 0; q < Q; q++) {
                if (delta[T - 1][q] <= max_value) {
                    continue;
                }
                max_value = delta[T - 1][q];
                max_index = q;
            }

            best_path[T - 1] = max_index;
            for (auto t = T - 2; t > 0; t--) {
                best_path[t] = psi[t + 1][best_path[t + 1]];
            }
            best_path[0] = psi[1][best_path[1]];

            return best_path;
        };
    private:
        static constexpr std::size_t PADDING = Q / 8;
        matrix<float, Q, Q> transition_matrix;
        array<array<float, Q>, T> delta;
        matrix<int, T, Q> psi;
        array<float, Q> weighting_vector;
        array<std::size_t, T> best_path;
    };
}


