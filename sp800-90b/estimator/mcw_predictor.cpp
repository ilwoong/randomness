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

#include "mcw_predictor.h"

#include <algorithm>

using namespace randomness::sp800_90b::estimator;

McwPredictorBinary::McwPredictorBinary(size_t windowSize)
{
    this->windowSize = windowSize;
    count.fill(0);
}

int16_t McwPredictorBinary::Predict()
{
    if (window.size() < windowSize) {
        return -1;
    }

    return count[0] > count[1] ? 0 : 1;
}

void McwPredictorBinary::PushBack(uint8_t sample)
{
    window.push_back(sample);
    count[sample] += 1;

    if (window.size() > windowSize) {
        count[window[0]] -= 1;
        window.erase(window.begin());
    }
}

McwPredictorLiteral::McwPredictorLiteral(size_t windowSize)
{
    this->windowSize = windowSize;
    maxCount = 0;
    count.fill(0);
}

int16_t McwPredictorLiteral::Predict()
{
    if (window.size() < windowSize) {
        return -1;
    }

    return mcv;
}

void McwPredictorLiteral::PushBack(uint8_t sample)
{
    window.push_back(sample);
    count[sample] += 1;

    if (window.size() > windowSize) {
        auto head = window[0];
        count[head] -= 1;
        window.erase(window.begin());

        if (mcv == head) {
            InvalidateMostCommonValue();
        }
    }

    if (maxCount <= count[sample]) {
        mcv = sample;
        maxCount = count[sample];
    }
}

void McwPredictorLiteral::InvalidateMostCommonValue() 
{
    maxCount = *std::max_element(count.begin(), count.end());
    
    auto iter = window.rbegin();
    while (count[*iter] != maxCount) {
        std::advance(iter, 1);
    }

    mcv = *iter;
}