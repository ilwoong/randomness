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
    sample = data;
    countSamples = len;
    countAlphabets = alph_size;
    CountCorrectPredictions();
    return EstimateByPrediction();
}

double PredictionEstimator::EstimateByPrediction()
{
    auto max = 1.0 / countAlphabets;
    auto p_global = static_cast<double>(countCorrects) / static_cast<double>(countPredictions);
    auto p_global_prime = UpperBoundProbability(p_global, countPredictions);

    max = std::max(max, p_global_prime);
    if (max < 1.0) {
        try {
            auto p_local = BinarySearch(logl(0.99), 0.0, 1.0, maxCorrectRuns + 1);
            max = std::max(max, p_local);
            logstream << ", p_local=" << p_local;
            
        } catch (std::exception e) {
            logstream << ", exception=" << e.what();
        }
    }

    logstream << ", p_global=" << p_global_prime;
    logstream << ", p_max=" << max;

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

void PredictionEstimator::CountCorrectPredictions()
{
    Initialize();

    for (size_t i = 1; i < countSamples; ++i) {
        UpdatePrediction(sample[i]);
        UpdateScoreBoard(sample[i]);
    }

    if (maxCorrectRuns < correctRuns) {
        maxCorrectRuns = correctRuns;
    }

    logstream << "countCorrects=" << countCorrects << ", max_run=" << maxCorrectRuns;
}

void PredictionEstimator::CountCorrects(uint8_t feed)
{
    if (frequent[winner] == feed) {
        correctRuns += 1;
        countCorrects += 1;

    } else {
        if (maxCorrectRuns < correctRuns) {
            maxCorrectRuns = correctRuns;
        }
        correctRuns = 0;
    }
}

void PredictionEstimator::UpdateScoreBoard(uint8_t feed) 
{
    for (auto i = 0; i < 4; ++i) {
        if (frequent[i] == feed) {
            scoreboard[i] += 1;

            if (scoreboard[i] >= scoreboard[winner]) {
                winner = i;
            }
        }
    }
}