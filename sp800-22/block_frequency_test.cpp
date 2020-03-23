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

#include "block_frequency_test.h"

#include "../algorithm/numerical_recipes.h"
#include "../common/hamming_weight.h"

using namespace randomness::algorithm;
using namespace randomness::sp800_22;

const std::string BlockFrequencyTest::Name() const
{
    return "Frequency test within a block";
}

const std::string BlockFrequencyTest::ShortName() const
{
    return "Blk.Freq.";
}

static double CalculateStatistic(const Sample& sample, size_t blockLength, size_t countBlocks)
{
    double squared_sum = 0;
    for (auto i = 0; i < countBlocks; ++i) {
        auto hw = HammingWeight(sample, i * blockLength, blockLength);
        auto pi = hw / static_cast<double>(blockLength);
        squared_sum += (pi - 0.5) * (pi - 0.5);
    }
    auto chisquare = 4 * blockLength * squared_sum;

    return chisquare;
}

std::vector<randomness_result_t> BlockFrequencyTest::Evaluate(const Sample& sample)
{
    blockLength = 128;    
    auto countBlocks = sample.BinaryData().size() / blockLength;
    auto chisquare = CalculateStatistic(sample, blockLength, countBlocks);
    auto pvalue = igammac(countBlocks/2.0, chisquare/2.0);

    auto result = std::vector<randomness_result_t>{};
    result.push_back(randomness_result_t {Name(), ShortName(), "m = 128", pvalue});

    logstream << "countBlocks = " << countBlocks << ", 𝛘² = " << chisquare;
    
    return result;
}