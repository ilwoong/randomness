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

#include "lag_prediction_estimator.h"

#include <vector>

using namespace randomness::sp800_90b::estimator;

std::string LagPredictionEstimator::Name() const
{
    return "Lag Prediction Estimate";
}

void LagPredictionEstimator::Initialize()
{
    countPredictions = countSamples - 1;
    
    winner = 0;
    correctRuns = 0;
    maxCorrectRuns = 0;

    frequent.assign(128, -1);
    scoreboard.assign(128, 0);

    frequent[127] = sample[0];
}

void LagPredictionEstimator::UpdatePrediction(uint8_t feed)
{
    CountCorrects(feed);

    frequent.push_back(feed);
    frequent.erase(frequent.begin());
}