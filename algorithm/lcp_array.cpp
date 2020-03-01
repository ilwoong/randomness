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

#include "lcp_array.h"

#include <algorithm>

using namespace randomness::algorithm;

LcpArray LcpArray::Create(const SuffixArray& sa)
{
	LcpArray lcp;
	lcp.Build(sa);
	return lcp;
}

LcpArray LcpArray::Create(const uint8_t* data, size_t length)
{
	auto sa = SuffixArray::Create(data, length);
	return Create(sa);
}

void LcpArray::Build(const SuffixArray& sa)
{
	auto data = sa.RawData();
	auto length = sa.Length();

	lcp_array.clear();
	lcp_array.assign(length, 0);

	auto rank = std::vector<int64_t>(length + 1, 0);
	for (size_t i = 0; i < length; ++i) {
		rank[sa[i]] = i + 1;
	}

	size_t lcp = 0;
	for (size_t i = 0; i < length; ++i) {
		auto rk = rank[i];
		if (rk > 1) {
			auto j = sa[rk];
			while (sa.EqualTo(i, j, lcp)) {
				lcp += 1;
			}
			lcp_array[rk] = lcp;
		}

		if (lcp > 0) {
			lcp -= 1;
		}
	}

	max_lcp = static_cast<size_t>(*std::max_element(lcp_array.begin(), lcp_array.end()));
}

size_t LcpArray::operator[](size_t pos) const
{
	return lcp_array[pos];
}

const std::vector<size_t>& LcpArray::Array() const
{
	return lcp_array;
}

size_t LcpArray::Max() const
{
	return max_lcp;
}