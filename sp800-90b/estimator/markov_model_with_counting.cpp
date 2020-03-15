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

#include "markov_model_with_counting.h"

#include <algorithm>

using namespace randomness::sp800_90b::estimator;

void MarkovModelWithCounting::init(const uint8_t* sample, size_t order)
{
    trace.assign(sample, sample + order);

    chain.clear();
    entries = 0;

    new_entry(sample[order]);
}

void MarkovModelWithCounting::new_entry(uint8_t sample) 
{  
    if (entries >= MaxEntries) {
        return;
    }

    chain[trace] = McvTracker();
    chain[trace].Create(sample);
    entries += 1;

    update_trace(sample);
}

int16_t MarkovModelWithCounting::update_entry(chain_t::iterator& iter, uint8_t sample)
{
    auto prediction = iter->second.MostCommonValue();
    iter->second.Update(sample);
    update_trace(sample);
    return prediction;
}

int16_t MarkovModelWithCounting::predict(uint8_t sample)
{
    auto prediction = static_cast<int16_t>(-1);
    
    auto iter = chain.find(trace);
    if (iter != chain.end()) {
        prediction = update_entry(iter, sample);
    } 
    else {
        new_entry(sample);
    }

    return prediction;
}

void MarkovModelWithCounting::update_trace(uint8_t sample) 
{
    trace.push_back(sample);
    trace.erase(trace.begin());
}


void MarkovModelWithCountingBinary::init(const uint8_t* sample, size_t order)
{
    mask = 0;
    trace = 0;
    for (auto i = 0; i < order; ++i) {
        mask <<= 1;
        mask ^= 0x1;
        
        trace <<= 1;
        trace ^= sample[i];
    }
    trace &= mask;

    chain.clear();
    entries = 0;

    new_entry(sample[order]);
}

void MarkovModelWithCountingBinary::new_entry(uint8_t sample) 
{  
    if (entries >= MaxEntries) {
        return;
    }

    chain[trace] = McvTracker();
    chain[trace].Create(sample);
    entries += 1;

    update_trace(sample);
}

int16_t MarkovModelWithCountingBinary::update_entry(binary_chain_t::iterator& iter, uint8_t sample)
{
    auto prediction = iter->second.MostCommonValue();
    iter->second.Update(sample);
    update_trace(sample);
    return prediction;
}

int16_t MarkovModelWithCountingBinary::predict(uint8_t sample)
{
    auto prediction = static_cast<int16_t>(-1);
    
    auto iter = chain.find(trace);
    if (iter != chain.end()) {
        prediction = update_entry(iter, sample);
    } 
    else {
        new_entry(sample);
    }

    return prediction;
}

void MarkovModelWithCountingBinary::update_trace(uint8_t sample) 
{
    trace <<= 1;
    trace ^= sample;
    trace &= mask;
}