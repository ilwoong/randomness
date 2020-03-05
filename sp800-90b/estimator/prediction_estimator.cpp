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

#include "prediction_estimator.h"

#include <algorithm>
#include <cmath>

using namespace randomness::sp800_90b::estimator;

static const double LogAlpha = log(0.99);

double PredictionEstimator::Estimate(const uint8_t* data, size_t len, size_t alph_size) 
{
    CountCorrectPredictions(data, len, alph_size);
    return EstimateByPrediction(len);
}

double PredictionEstimator::EstimateByPrediction(size_t k)
{
    auto max = 1.0 / k;
    auto p_global = static_cast<double>(countCorrects) / static_cast<double>(countPredictions);
    auto p_global_prime = UpperBoundProbability(p_global, countPredictions);

    max = std::max(max, p_global_prime);
    if (max < 1.0) {
        auto p_local = BinarySearch(maxCorrectRuns + 1, 0.5, 1.0);
        max = std::max(max, p_local);
    }

    return -log2(max);
}

double PredictionEstimator::EvaluateBinarySearch(double p, double r) const
{
    auto q = 1.0 - p;
    auto x = 1.0;

    for (auto i = 0; i < 10; ++i) {
        x = 1.0 + q * powl(p, r) * powl(x, r + 1.0);
    }

    return logl(1.0 - p * x) - logl((r + 1 - r * x) * q) - (countPredictions + 1.0) * logl(x);
}