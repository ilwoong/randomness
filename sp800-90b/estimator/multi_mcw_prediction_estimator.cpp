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

#include "multi_mcw_prediction_estimator.h"

#include <algorithm>
#include <vector>

using namespace randomness::sp800_90b::estimator;

static constexpr std::array<size_t, 4> WindowSize = {63, 255, 1023, 4095};

std::string MultiMcwPredictionEstimator::Name() const
{
    return "MultiMCW Prediction Estimate";
}

void MultiMcwPredictionEstimator::Initialize()
{
    startPredictionIndex = WindowSize[0];
    countPredictions = countSamples - startPredictionIndex;

    prediction.assign(4, -1);
    scoreboard.assign(4, 0);

    mcw.clear();
    for (auto i = 0; i < 4; ++i) {
        if (countAlphabets == 2) {
            mcw.push_back(std::make_shared<McwPredictorBinary>(WindowSize[i]));
        }
        else {
            mcw.push_back(std::make_shared<McwPredictorLiteral>(WindowSize[i]));
        }
    }

    for (auto i = 0; i < WindowSize[0]; ++i) {
        for (auto& window : mcw) {
            window->PushBack(sample[i]);
        }
    }
}

void MultiMcwPredictionEstimator::UpdatePredictions(size_t idx)
{
    for (auto j = 0; j < 4; ++j) {
        prediction[j] = mcw[j]->Predict();
        mcw[j]->PushBack(sample[idx]);
    }
}