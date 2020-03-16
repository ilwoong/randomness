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

#include "mmc_predictor.h"

#include <algorithm>

using namespace randomness::sp800_90b::estimator;

static constexpr size_t MaxEntries = 100000;

void MmcPredictor::Initialize(const uint8_t* sample, size_t order)
{
    this->order = order;
    entries = 0;
}

void MmcPredictorBinary::Initialize(const uint8_t* sample, size_t order)
{
    MmcPredictor::Initialize(sample, order);

    mask = 0;
    trace = 0;
    for (auto d = 0; d < order; ++d) {
        mask = (mask << 1) ^ 0x1;
        trace = (trace << 1) ^ sample[d];
    }

    chain.resize(1 << order);
    CreateEntry(sample[order]);
}

void MmcPredictorBinary::CreateEntry(uint8_t sample)
{
    if (entries >= MaxEntries) {
        return;
    }

    chain[trace].Create(sample);
    entries += 1;

    UpdateTrace(sample);
}

int16_t MmcPredictorBinary::Predict(uint8_t sample)
{
    int16_t prediction = chain[trace].MostCommonValue();

    if (prediction != -1) {
        chain[trace].Update(sample);
        UpdateTrace(sample);
    } 
    else {
        CreateEntry(sample);
    }

    return prediction;
}

void MmcPredictorBinary::UpdateTrace(uint8_t sample)
{
    trace = (trace << 1) ^ sample;
    trace &= mask;
}

void MmcPredictorLiteral::Initialize(const uint8_t* sample, size_t order)
{
    MmcPredictor::Initialize(sample, order);

    trace.assign(sample, sample + order);
    
    chain.clear();
    CreateEntry(sample[order]);
}

void MmcPredictorLiteral::CreateEntry(uint8_t sample)
{
    if (entries >= MaxEntries) {
        return;
    }

    chain[trace] = McvTracker();
    chain[trace].Create(sample);
    entries += 1;

    UpdateTrace(sample);
}

int16_t MmcPredictorLiteral::Predict(uint8_t sample)
{
    int16_t prediction = -1;

    auto iter = chain.find(trace);
    if (iter != chain.end()) {
        prediction = iter->second.MostCommonValue();
        iter->second.Update(sample);
        UpdateTrace(sample);
    } 
    else {
        CreateEntry(sample);
    }

    return prediction;
}

void MmcPredictorLiteral::UpdateTrace(uint8_t sample)
{
    trace.push_back(sample);
    trace.erase(trace.begin());
}