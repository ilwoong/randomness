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

#include "tuple_estimator.h"

#include <cmath>

using namespace randomness::sp800_90b::estimator;

std::string TupleEstimator::Name() const
{
    return "t-Tuple Estimator";
}

double TupleEstimator::Estimate(const uint8_t* data, size_t len, size_t alph_size)
{    
    auto lcp = LcpArray::Create(data, len);

    // Let Q[i] store the number of occurrences of the most common i-tuple in S
    auto Q = GetMaximumTupleCounts(lcp, len);
    auto pmax = CalculateMaximumProbability(Q, len);

    if (verbose) {
        logstream << "t=" << u - 1 << ", pmax=" << pmax;
    }

    return UpperBoundProbability(pmax, len);
}

double TupleEstimator::CalculateMaximumProbability(const std::vector<size_t>& Q, size_t length) const
{
    auto pmax = -1.0;
    for (size_t i = 1; i < u; ++i) {
        auto p = Q[i] / static_cast<double>(length - i);
        p = pow(p, 1.0 / i);

        if (pmax < p) {
            pmax = p;
        }
    }

    return pmax;
}

std::vector<size_t> TupleEstimator::GetMaximumTupleCounts(const LcpArray& lcp, size_t length)
{
    auto max_lcp = lcp.Max();
    auto Q = std::vector<size_t>(max_lcp + 1, 1);
    auto A = std::vector<size_t>(max_lcp + 2, 0);
    auto I = std::vector<size_t>(max_lcp + 3, 0);

    int64_t j = 0;
    for (size_t i = 1; i <= length; ++i) {
        size_t c = 0;
        if (lcp[i] < lcp[i - 1]) {
            auto t = lcp[i - 1];
            --j;

            while (t > lcp[i]) {
                if ((j > 0) && I[j] == t) {
                    A[I[j]] += A[I[j + 1]];
                    A[I[j + 1]] = 0;
                    --j;
                }

                if (Q[t] >= A[I[j + 1]] + 1) {
                    t = (j > 0) ? I[j] : lcp[i];
                }
                else {
                    Q[t--] = A[I[j + 1]] + 1;
                }
            }

            c = A[I[j + 1]];
            A[I[j + 1]] = 0;
        }

        if (lcp[i] > 0) {
            if ((j < 1) || (I[j] < lcp[i])) {
                I[++j] = lcp[i];
            }

            A[I[j]] += c + 1;
        }
    }

    u = 1;
    while ((Q[u] >= 35) && ((u++) < max_lcp));

    return Q;
}
