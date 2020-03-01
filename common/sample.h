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

#ifndef _RANDOMNESS_COMMON_SAMPLE_H__
#define _RANDOMNESS_COMMON_SAMPLE_H__

#include <cstdint>
#include <vector>

namespace randomness { namespace common {

    class Sample
    {
    private:
        std::vector<uint8_t> binSymbols;
        std::vector<uint8_t> octSymbols;

    public:
        void AppendBit(uint8_t value);        
        void AppendByte(uint8_t value);
        void AppendBytes(const uint8_t* data, size_t length);

        size_t BitLength() const;
        size_t ByteLength() const;

        const uint8_t* BitData() const;
        const uint8_t* ByteData() const;
    };
}}

#endif