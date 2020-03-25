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

#include "runs_test.h"

#include "../common/hamming_weight.h"

using namespace randomness::sp800_22;
using namespace randomness::common;

const std::string RunsTest::Name() const
{
    return "Runs test";
}

const std::string RunsTest::ShortName() const
{
    return "Runs";
}

size_t RunsTest::MinimumLengthInBits() const 
{
    return 100;
}

static size_t TotalNumberOfOnes(const Sample& sample)
{
    auto data = sample.OctalData();
    auto length = data.size();

    size_t vobs = 1;
    uint8_t idx = 0;
    for (auto k = 0; k < length - 1; ++k) {
        idx = data[k] ^ ((data[k] << 1) ^ (data[k + 1] >> 7));
        vobs += HammingWeight(idx);
    }
    idx = data[length - 1] ^ (data[length - 1] << 1) ^ (data[length - 1] & 0x1);
    vobs += HammingWeight(idx);

    return vobs;
}

static double CalculateStatistic(const Sample& sample, double pi)
{
    auto vobs = TotalNumberOfOnes(sample);
    auto term = pi * (1.0 - pi);

    auto length = sample.BinaryData().size();
    auto numerator = abs(vobs - (2 * length * term));
    auto denominator = 2 * sqrt(2 * length) * term;

    return numerator / denominator;
}

std::vector<randomness_result_t> RunsTest::Evaluate(const Sample& sample)
{
    auto length = sample.BinaryData().size();
    auto pi = HammingWeight(sample) / static_cast<double>(length);
    auto tau = 2.0 / length;

    auto pvalue = 0.0;
    if (abs(pi - 0.5) < tau) {
        auto fraction = CalculateStatistic(sample, pi);
        pvalue = std::erfc(fraction);

        logstream << "𝜋 = " << pi << ", 𝜏 = " << tau << ", fraction = " << fraction;
    }
    else {
        pvalue = 0.0;
        
        logstream << "monobit test failed";
    }

    auto result = std::vector<randomness_result_t>{};
    result.push_back(randomness_result_t {Name(), ShortName(), "", pvalue});
    
    return result;
}