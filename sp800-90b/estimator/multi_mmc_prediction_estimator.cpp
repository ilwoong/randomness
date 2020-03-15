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

#include <algorithm>

#include <omp.h>

using namespace randomness::sp800_90b::estimator;

using history_t = std::vector<uint8_t>;
using counter_t = std::array<size_t, 256>;
using chain_t = std::map<history_t, counter_t>;

std::string MultiMmcPredictionEstimator::Name() const
{
    return "MultiMMC Prediction Estimate";
}

void MultiMmcPredictionEstimator::Initialize()
{
    startPredictionIndex = 2;
    countPredictions = countSamples - startPredictionIndex;

    winner = 0;
    countCorrects = 0;
    correctRuns = 0;
    maxCorrectRuns = 0;

    prediction.assign(16, -1);
    scoreboard.assign(16, 0);

    for (auto d = 0; d < 16; ++d) {
        if (countAlphabets == 2) {
            mmc_binary[d].init(sample, d + 1);
        } 
        else {
            mmc[d].init(sample, d + 1);
        }
    }
}

void MultiMmcPredictionEstimator::UpdatePredictions(size_t idx)
{
    auto min = std::min(16, static_cast<int>(idx - 1));
    auto feed = sample[idx];

    for (auto d = 0; d < min; ++d) {
        prediction[d] = countAlphabets == 2 ? mmc_binary[d].predict(feed) : mmc[d].predict(feed);
        
        if (prediction[d] == -1) {
            for (auto i = d + 1; i < min; ++i) {
                if (countAlphabets == 2) {
                    mmc_binary[i].new_entry(feed);
                } 
                else {
                    mmc[i].new_entry(feed);
                }
                
            }
            break;
        }
    }
}

