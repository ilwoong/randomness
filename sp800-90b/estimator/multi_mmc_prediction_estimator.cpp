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

#include "multi_mmc_prediction_estimator.h"

using namespace randomness::sp800_90b::estimator;

static constexpr size_t CountPredictors = 16;

std::string MultiMmcPredictionEstimator::Name() const
{
    return "MultiMMC Prediction Estimate";
}

void MultiMmcPredictionEstimator::Initialize()
{
    startPredictionIndex = 2;
    countPredictions = countSamples - startPredictionIndex;

    prediction.assign(CountPredictors, -1);
    scoreboard.assign(CountPredictors, 0);

    mmc.clear();
    for (auto d = 0; d < CountPredictors; ++d) {
        if (countAlphabets == 2) {
            mmc.push_back(std::make_shared<MmcPredictorBinary>());
        }
        else {
            mmc.push_back(std::make_shared<MmcPredictorLiteral>());
        }
        mmc[d]->Initialize(sample, d + 1);
    }
}

void MultiMmcPredictionEstimator::UpdatePredictions(size_t idx)
{
    auto min = std::min(CountPredictors, idx - 1);
    auto feed = sample[idx];

    for (auto d = 0; d < min; ++d) {
        prediction[d] = mmc[d]->Predict(feed);
        
        if (prediction[d] == -1) {
            for (auto i = d + 1; i < min; ++i) {
                mmc[i]->CreateEntry(feed);
            }
            break;
        }
    }
}

