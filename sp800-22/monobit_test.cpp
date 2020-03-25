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

#include "monobit_test.h"

#include "../common/hamming_weight.h"

using namespace randomness::sp800_22;
using namespace randomness::common;

static const double SQRT2 = sqrt(2);

const std::string MonobitTest::Name() const
{
    return "Monobit test";
}

const std::string MonobitTest::ShortName() const
{
    return "Monobit";
}

size_t MonobitTest::MinimumLengthInBits() const 
{
    return 100;
}

std::vector<randomness_result_t> MonobitTest::Evaluate(const Sample& sample)
{
    auto length = sample.BinaryData().size();
    auto hw = HammingWeight(sample);
    auto sobs = (2 * hw - length) / sqrt(length);
    auto pvalue = std::erfc(sobs / SQRT2);

    logstream << "count 1s = " << hw << ", Sobs = " << sobs;

    auto result = std::vector<randomness_result_t>{};
    result.push_back(randomness_result_t {Name(), ShortName(), "", pvalue});
    
    return result;
}