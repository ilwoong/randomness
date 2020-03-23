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

#include "sample.h"

using namespace randomness::common;

void Sample::AppendBit(uint8_t value)
{
    binarySymbols.push_back(value);
}

void Sample::AppendByte(uint8_t value)
{
    octalSymbols.push_back(value);
    for (auto i = 0; i < 8; ++i) {
        binarySymbols.push_back((value & 0x80) >> 7);
        value <<= 1;
    }
}

void Sample::AppendBytes(const uint8_t* data, size_t length)
{
    for (size_t i = 0; i < length; ++ i) {
        AppendByte(data[i]);
    }
}

const std::vector<uint8_t>& Sample::BinaryData() const
{
    return binarySymbols;
}

const std::vector<uint8_t>& Sample::OctalData() const
{
    return octalSymbols;
}