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

using namespace randomness::sp800_90b::estimator;

std::string LrsEstimator::Name() const
{
	return "LRS Estimator";
}

double LrsEstimator::Estimate(const uint8_t* data, size_t len, size_t alph_size)
{	
	auto lcp = LcpArray::Create(data, len);
	auto S = GetLRS(lcp, len);
	auto pmax = CalculateMaximumProbability(S, lcp.Max(), len);

	if (verbose) {
		logstream << "u=" << u << ", v=" << lcp.Max() << ", pmax=" << pmax;
	}

	return UpperBoundProbability(pmax, len);
}

double LrsEstimator::CalculateMaximumProbability(const std::vector<int64_t>& S, size_t v, size_t length) const
{
	auto pmax = 0.0;
	for (size_t i = u; i <= v; ++i) {
		auto p = S[i] / static_cast<double>(((length - i) * (length - i + 1)) >> 1);
		p = pow(p, 1.0 / i);

		if (pmax < p) {
			pmax = p;
		}
	}

	return pmax;
}

std::vector<int64_t> LrsEstimator::GetLRS(const LcpArray& lcp, size_t length)
{	
	auto max_lcp = lcp.Max();

	auto Q = GetMaximumTupleCounts(lcp, length);
	auto A = std::vector<int64_t>(max_lcp + 2, 0);
	auto S = std::vector<int64_t>(max_lcp + 1, 0);

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
