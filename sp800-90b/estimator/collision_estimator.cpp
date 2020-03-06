/**
 * The MIT License
 *
 * Copyright (c) 2020 Ilwoong Jeong (https://github.com/ilwoong)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "collision_estimator.h"

#include <cmath>
#include <stdexcept>

using namespace randomness::sp800_90b::estimator;

std::string CollisionEstimator::Name() const
{
    return "Collision Estimate";
}

double CollisionEstimator::Estimate(const uint8_t* data, size_t len, size_t alph_size)
{
    if (alph_size != 2) {
        throw std::invalid_argument("only applicable to binary data");
    }

    auto lower_bound_mean = FromBinaryCollisions(data, len);

    auto prob = 0.0;
    try {
        prob = BinarySearch(lower_bound_mean, 0.5, 1.0);
        logstream << ", p=" << prob;

    } catch (std::exception e) {
        logstream << ", exception=" << e.what();
        return 1.0;
    }

    return -log2(prob);
}

double CollisionEstimator::FromBinaryCollisions(const uint8_t* data, size_t len)
{
    auto count_collisions = 0;

    size_t index = 0;
    size_t offset_to_first_collision = 0;
    size_t squared_sum = 0;
    
    while (index < len - 1) {
        if (data[index] == data[index + 1]) {
            offset_to_first_collision = 2;
            
        } else if (index < len - 2) {
            offset_to_first_collision = 3;

        } else {
            break;
        }

        index += offset_to_first_collision;
        squared_sum += offset_to_first_collision * offset_to_first_collision;
        count_collisions += 1;
    }

    auto mean = static_cast<double>(index) / count_collisions;
    auto std_dev = sqrt((squared_sum - index * mean) / (count_collisions - 1));
    auto lower_bound_mean = mean - Zalpha * sqrt(std_dev / count_collisions);

    logstream << "X̄=" << mean << ", σ^=" << std_dev << ", X̄'=" << lower_bound_mean;

    return lower_bound_mean;
}

double CollisionEstimator::EvaluateBinarySearch(double p, double ignored) const
{
    auto q = 1.0 - p;
    auto pinv = 1.0 / p;
    auto qinv = 1.0 / q;
    auto diff = 0.5 * (pinv - qinv);

    auto Fq = q * (2.0 * q * q + 2.0 * q + 1);
    
    auto lhs = (p * qinv * qinv) * (1.0 + diff) * Fq;
    auto rhs = p * qinv * diff;

    return lhs - rhs;
}