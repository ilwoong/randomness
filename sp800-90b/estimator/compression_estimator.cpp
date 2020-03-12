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

#include "compression_estimator.h"

#include <cmath>
#include <vector>

#include "binary_search.h"
#include "boundary.h"

using namespace randomness::sp800_90b::estimator;

std::string CompressionEstimator::Name() const
{
    return "Compression Estimate";
}

double CompressionEstimator::Estimate(const uint8_t* data, size_t len, size_t alph_size)
{
    auto func = std::bind(&CompressionEstimator::EvaluateBinarySearch, this, std::placeholders::_1, std::placeholders::_2);
    BinarySearch search;
    search.SetFunction(func);

    auto mean = Calculate(data, len);
    auto entropy = 1.0;
    
    try {
        auto p = search.FindSolution(mean, 1.0 / (1 << blockLength), 1.0);
        logstream << ", p=" << p;
        entropy = -log2(p) / static_cast<double>(blockLength);

    } catch (std::exception e) {
        logstream << e.what();
    }

    return entropy;
}

static size_t bitstring2index(const uint8_t* data, size_t offset, size_t length)
{
    size_t index = data[offset];
    for (auto i = offset + 1; i < offset + length; ++i) {
        index <<= 1;
        index |= data[i];
    }
    return index;
}

double CompressionEstimator::Calculate(const uint8_t* data, size_t len)
{
    countBlocks = len / blockLength;
    countTestBlocks = countBlocks - countInitBlocks;

    auto dict = std::vector<size_t>(1 << blockLength, 0);
    for (size_t i = 0; i < countInitBlocks; ++i) {
        auto index = bitstring2index(data, i * blockLength, blockLength);
        dict[index] = i + 1;
    }

    auto sum = 0.0;
    auto squared_sum = 0.0;
    for (size_t i = countInitBlocks; i < countBlocks; ++i) {
        auto index = bitstring2index(data, i * blockLength, blockLength);
        auto log2D = log2((i + 1) - dict[index]);

        sum += log2D;
        squared_sum += log2D * log2D;

        dict[index] = i + 1;
    }

    auto mean = static_cast<double>(sum) / countTestBlocks;
    auto std_dev = 0.5907 * sqrt((squared_sum / (countTestBlocks - 1)) - mean * mean);
    auto lower_bound_mean = LowerBoundMean(mean, std_dev, countTestBlocks);

    logstream << "X̄=" << mean << ", σ^=" << std_dev << ", X̄'=" << lower_bound_mean;

    return lower_bound_mean;
}

double CompressionEstimator::EvaluateBinarySearch(double p, double ignored) const
{
    double coef = static_cast<double>((1 << blockLength) - 1);
    double q = (1.0 - p) / coef;

    return G(p) + coef * G(q);
}

double CompressionEstimator::G(double z) const
{ 
    auto q = 1.0 - z;
    auto powq = 1.0;

    auto innerSum = 0.0;
    for (size_t u = 1; u < countInitBlocks + 1; ++u) {
        innerSum += log2(u) * powq;
        powq *= q;
    }    
    innerSum *= (countBlocks - countInitBlocks);

    auto finalSum = 0.0;
    for (size_t t = countInitBlocks + 1; t <= countBlocks; ++t) {
        auto tmp = log2(t) * powq;

        innerSum += (countBlocks - t) * tmp;
        finalSum += tmp;

        powq *= q;
    }

    return ((z * z * innerSum) + (z * finalSum)) / static_cast<double>(countTestBlocks);
}
