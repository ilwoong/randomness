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

#include "lrs_estimator.h"

#include <cmath>

#include "boundary.h"

using namespace randomness::sp800_90b::estimator;

std::string LrsEstimator::Name() const
{
    return "LRS Estimate";
}

double LrsEstimator::Estimate(const uint8_t* data, size_t len, size_t alph_size)
{    
    auto lcp = LcpArray::Create(data, len);
    return Estimate(lcp);
}

static inline size_t FindSmallestU(const std::vector<size_t>& Q, size_t max_lcp)
{
    size_t u = 1;
    while ((Q[u] < 35) && ((u++) < max_lcp));
    return u;
}

double LrsEstimator::Estimate(const LcpArray& lcp)
{
    auto len = lcp.Array().size();
    auto Q = GetMaximumTupleCounts(lcp, len);
    auto u = FindSmallestU(Q, lcp.Max());
    auto v = lcp.Max();

    auto C = GetLRS(lcp, u, len);
    auto pmax = CalculateMaximumProbability(C, u, v, len);

    logstream << "u=" << u << ", v=" << lcp.Max() << ", pmax=" << pmax;

    return -log2(UpperBoundProbability(pmax, len));
}

double LrsEstimator::CalculateMaximumProbability(const std::vector<size_t>& C, size_t u, size_t v, size_t length) const
{
    auto pmax = 0.0;
    for (size_t i = u; i <= v; ++i) {
        auto p = C[i] / static_cast<double>(((length - i) * (length - i + 1)) >> 1);
        p = pow(p, 1.0 / i);

        if (pmax < p) {
            pmax = p;
        }
    }

    return pmax;
}

/**
 * Based on the code below:
 * https://github.com/usnistgov/SP800-90B_EntropyAssessment/blob/master/cpp/shared/lrs_test.h
 */
std::vector<size_t> LrsEstimator::GetLRS(const LcpArray& lcp, size_t u, size_t length)
{    
    auto max_lcp = lcp.Max();

    auto A = std::vector<size_t>(max_lcp + 2, 0);
    auto S = std::vector<size_t>(max_lcp + 1, 0);

    for (size_t i = 1; i <= length; ++i) {
        if ((lcp[i - 1] >= u) && (lcp[i] < lcp[i - 1])) {
            auto b = lcp[i];

            if (b < u) {
                b = u - 1;
            }

            for (auto t = lcp[i - 1]; t > b; --t) {
                A[t] += A[t + 1];
                A[t + 1] = 0;

                S[t] += ((A[t] + 1) * A[t]) >> 1;
            }

            if (b >= u) {
                A[b] += A[b + 1];
            }

            A[b + 1] = 0;
        }

        if (lcp[i] >= u) {
            A[lcp[i]]++;
        }
    }

    return S;
}
