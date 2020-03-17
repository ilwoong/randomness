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

#include "lz78y_prediction_estimator.h"
#include "prediction_estimator.h"

using namespace randomness::sp800_90b::estimator;

static constexpr size_t MaxEntries = 65536;

std::string Lz78yPredictionEstimator::Name() const
{
    return "LZ78Y Prediction Estimate";
}

void Lz78yPredictionEstimator::Initialize()
{
    entries = 0;
    startPredictionIndex = 17;
    countPredictions = countSamples - 16 - 1;

    prediction.assign(1, -1);
    scoreboard.clear();

    dictionary.clear();
    for (auto i = 0; i < 16; ++i) {
        if (countAlphabets == 2) {
            dictionary.push_back(std::make_shared<Lz78yPredictorBinary>());
        } 
        else {
            dictionary.push_back(std::make_shared<Lz78yPredictorLiteral>());
        }
        
        dictionary[i]->Initialize(sample, 15 - i);
    }
    entries += 16;
}

void Lz78yPredictionEstimator::UpdatePredictions(size_t idx)
{
    prediction[0] = -1;
    auto max = 0;

    for (auto i = 15; i >= 0; --i) {
        auto result = dictionary[i]->Predict(sample[idx]);

        if (result.MostCommonValue() != -1) {
            if ((max < result.MostCommonCounter()) || ((max == result.MostCommonCounter()) && (prediction[0] < result.MostCommonValue()))) {
                prediction[0] = result.MostCommonValue();
                max = result.MostCommonCounter();
            }
        }
        else if (entries < MaxEntries) {
            dictionary[i]->CreateEntry(sample[idx]);
            entries += 1;
        }
    }

    for (auto i = 0; i < 16; ++i) {
        dictionary[i]->UpdateTrace(sample[idx]);
    }
}
