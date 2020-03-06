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

#include "markov_estimator.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

using namespace randomness::sp800_90b::estimator;

std::string MarkovEstimator::Name() const
{
    return "Markov Estimate";
}

static std::array<size_t, 5> CountPatterns(const uint8_t* data, size_t len)
{
    std::array<size_t, 5> counts = {0, 0, 0, 0, 0};

    size_t idx = data[0];
    counts[5] = data[0];
    for (size_t i = 1; i < len; ++i) {
        idx = (idx << 1 | data[i]) & 0b11;
        counts[idx] += 1;

        // counts[4] <- number of 1's in data
        counts[4] += data[i];
    }

    return counts;
}

static std::vector<double> GenerateProbabilities(const uint8_t* data, size_t len)
{
    auto counts = CountPatterns(data, len);
    std::array<double, 4> table{0, 0, 0, 0};

    auto p1 = counts[4] / static_cast<double>(len);
    auto p0 = 1.0 - p1;
    table[0] = counts[0] / static_cast<double>(counts[0] + counts[1]);
    table[1] = counts[1] / static_cast<double>(counts[0] + counts[1]);
    table[2] = counts[2] / static_cast<double>(counts[2] + counts[3]);
    table[3] = counts[3] / static_cast<double>(counts[2] + counts[3]);

    std::vector<double> probs;
    probs.push_back(p0 * pow(table[0], 127));
    probs.push_back(p0 * pow(table[1], 64) * pow(table[2], 63));
    probs.push_back(p0 * table[1] * pow(table[3], 126));
    probs.push_back(p1 * table[2] * pow(table[0], 126));
    probs.push_back(p1 * pow(table[2], 64) * pow(table[1], 63));
    probs.push_back(p1 * pow(table[3], 127));

    return probs;
}

double MarkovEstimator::Estimate(const uint8_t* data, size_t len, size_t alph_size)
{
    if (alph_size != 2) {
        throw std::invalid_argument("only applicable to binary data");
    }

    auto probs = GenerateProbabilities(data, len);
    auto pmax = *std::max_element(probs.begin(), probs.end());
    auto entropy = -log2(pmax) / 128.0;

    logstream << "pmax=" << pmax;

    return std::min(entropy, 1.0);
}