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

#ifndef __RANDOMNESS_ALGORITHM_SUFFIX_ARRAY_H__
#define __RANDOMNESS_ALGORITHM_SUFFIX_ARRAY_H__

#include <cstdint>
#include <vector>

namespace randomness { namespace algorithm {

    struct suffix_t {
        size_t index;
        size_t length;
        const uint8_t* suffix;

        friend bool operator<(const suffix_t& lhs, const suffix_t& rhs);
    };

    bool operator<(const suffix_t& lhs, const suffix_t& rhs);

    class SuffixArray {
    private:
        const uint8_t* data;
        size_t length;
        std::vector<suffix_t> suffix_array;
        
    public:
        static SuffixArray Create(const uint8_t* str, size_t length);

    private:
        void Build(const uint8_t* str, size_t length);

    public:
        size_t operator[](size_t pos) const;
        
        const std::vector<suffix_t>& Array() const;
        const uint8_t* RawData() const;
        size_t Length() const;

        bool EqualTo(size_t i, size_t j, size_t offset) const;
    };
}}

#endif