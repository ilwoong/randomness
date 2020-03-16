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

double Lz78yPredictionEstimator::Estimate(const uint8_t* data, size_t len, size_t alph_size)
{
    sample = data;
    countSamples = len;
    countAlphabets = alph_size;

    Initialize();

    for (auto i = startPredictionIndex; i < countSamples; ++i) {
        UpdatePredictions(i);
        CountCorrectPredictions(i);
    }

    if (maxCorrectRuns < correctRuns) {
        maxCorrectRuns = correctRuns;
    }

    logstream << "countCorrects=" << countCorrects << ", max_run=" << maxCorrectRuns;

    correct_info_t info = {countAlphabets, maxCorrectRuns, countCorrects, countPredictions};
    PredictionEstimator pe;
    return pe.Estimate(info);
}


void Lz78yPredictionEstimator::Initialize()
{
    entries = 0;
    countCorrects = 0;
    countPredictions = countSamples - 16 - 1;
    correctRuns = 0;
    maxCorrectRuns = 0;
    startPredictionIndex = 17;

    dictionary.clear();
    for (auto i = 0; i < 16; ++i) {
        if (countAlphabets == 2) {
            dictionary.push_back(std::make_shared<Lz78yPredictorBinary>());
        } else {
            dictionary.push_back(std::make_shared<Lz78yPredictorLiteral>());
        }
        
        dictionary[i]->Initialize(sample, 15 - i);
    }
    entries += 16;
}

void Lz78yPredictionEstimator::UpdatePredictions(size_t idx)
{
    prediction = -1;
    auto max = 0;

    for (auto i = 0; i < 16; ++i) {
        auto result = dictionary[i]->Predict(sample[idx]);

        if (result.MostCommonValue() == -1) {
            for (auto j = i; j < 16; ++j) {
                if (entries < MaxEntries) {
                    dictionary[j]->CreateEntry(sample[idx]);
                    entries += 1;
                }       
            }
            break;
        }        
        else if (max < result.MostCommonCounter()) {
            prediction = result.MostCommonValue();
            max = result.MostCommonCounter();
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

void Lz78yPredictionEstimator::CountCorrectPredictions(size_t idx)
{
    if (prediction == sample[idx]) {
        countCorrects += 1;
        correctRuns += 1;
    } 
    else {
        if (maxCorrectRuns < correctRuns) {
            maxCorrectRuns = correctRuns;
        }
        correctRuns = 0;
    }
}