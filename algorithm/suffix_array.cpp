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

#include "suffix_array.h"

#include <algorithm>
#include <cstring>

using namespace randomness::algorithm;

bool randomness::algorithm::operator<(const suffix_t& lhs, const suffix_t& rhs)
{
    const uint8_t* lptr = lhs.suffix;
    const uint8_t* rptr = rhs.suffix;

    int64_t cmp = memcmp(lptr, rptr, std::min(lhs.length, rhs.length));
    if (cmp == 0) {
        cmp = static_cast<int64_t>(lhs.length) - static_cast<int64_t>(rhs.length);
    }

    return cmp < 0;
}

static std::vector<suffix_t> BuildSuffixStructure(const uint8_t* str, size_t length) 
{
    std::vector<suffix_t> suffixes(length);

    for (size_t i = 0; i < length; ++i) {
        suffixes[i].index = i;
        suffixes[i].suffix = str + i;
        suffixes[i].length = length - i;
    }

    return suffixes;
}

SuffixArray SuffixArray::Create(const uint8_t* str, size_t length)
{
    SuffixArray sa;
    sa.Build(str, length);
    return sa;
}

void SuffixArray::Build(const uint8_t* str, size_t length)
{
    data = str;
    this->length = length;

    suffix_array = BuildSuffixStructure(str, length);
    std::sort(suffix_array.begin(), suffix_array.end());
}

size_t SuffixArray::operator[](size_t pos) const
{
    return suffix_array[pos].index;
}

const std::vector<suffix_t>& SuffixArray::Array() const
{
    return suffix_array;
}

const uint8_t* SuffixArray::RawData() const
{
    return data;
}

size_t SuffixArray::Length() const
{
    return length;
}

bool SuffixArray::EqualTo(size_t i, size_t j, size_t offset) const
{
    return (i + offset < length) && (j + offset < length) && (data[i + offset] == data[j + offset]);
}
