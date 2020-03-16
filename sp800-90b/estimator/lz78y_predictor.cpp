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

#include "lz78y_predictor.h"

using namespace randomness::sp800_90b::estimator;

void Lz78yPredictorBinary::Initialize(const uint8_t* sample, size_t order)
{
    mask = 0;
    trace = 0;
    for (auto i = order; i < 16; ++i) {
        mask = (mask << 1) ^ 0x1;
        trace = (trace << 1) ^ sample[i];
    }
    
    dictionary.resize(1 << (16 - order));
    dictionary[trace].Create(sample[16]);

    UpdateTrace(sample[16]);
}

McvTracker Lz78yPredictorBinary::Predict(uint8_t sample)
{
    McvTracker prediction = dictionary[trace];
    if (prediction.MostCommonValue() != -1) {
        dictionary[trace].Update(sample);
    }

    return prediction;
}

void Lz78yPredictorBinary::CreateEntry(uint8_t sample)
{
    dictionary[trace].Update(sample);
}

void Lz78yPredictorBinary::UpdateTrace(uint8_t sample)
{
    trace = (trace << 1) ^ sample;
    trace &= mask;
}

void Lz78yPredictorLiteral::Initialize(const uint8_t* sample, size_t order)
{
    trace.clear();
    for (auto i = order; i < 16; ++i) {
        trace.push_back(sample[i]);
    }
    dictionary[trace] = McvTracker();
    dictionary[trace].Create(sample[16]);

    UpdateTrace(sample[16]);
}

McvTracker Lz78yPredictorLiteral::Predict(uint8_t sample)
{
    McvTracker prediction;
    auto iter = dictionary.find(trace);
    if (iter != dictionary.end()) {
        prediction = iter->second;
        iter->second.Update(sample);
    }

    return prediction;
}

void Lz78yPredictorLiteral::CreateEntry(uint8_t sample)
{
    dictionary[trace] = McvTracker();
    dictionary[trace].Update(sample);
}

void Lz78yPredictorLiteral::UpdateTrace(uint8_t sample)
{
    trace.push_back(sample);
    trace.erase(trace.begin());
}