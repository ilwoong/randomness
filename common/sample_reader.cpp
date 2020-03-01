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

#include "sample_reader.h"

using namespace randomness::common;

static constexpr size_t ChunkSize = 4096;

void SampleReader::Open(const std::string& filepath)
{
    Close();

    ifs.open(filepath, std::ios::binary);
    if(ifs.is_open() == false) {
        throw std::string("file open failed: ") + filepath;
    }

    ifs.seekg(0, std::ios::end);
    length = static_cast<size_t>(ifs.tellg());
    ifs.seekg(0, std::ios::beg);
}

void SampleReader::Close()
{
    if (ifs.is_open()) {
        ifs.close();
    }
}

size_t SampleReader::Length() const
{
    return length;
}

SharePtrSample SampleReader::NextBits(size_t length)
{
    auto lenBytes = length >> 3;
    auto lenBits = length & 0x7;

    auto sample = NextBytes(lenBytes);
    char buffer[1] = {0};
    ifs.read(buffer, 1);

    uint8_t mask = 0x80;
    auto bits = static_cast<uint8_t>(buffer[0]);
    for (auto i = 0; i < lenBits; ++i) {
        sample->AppendBit((bits & 0x80) >> 7);
        bits <<= 1;
        mask = 0x80 | (mask >> 1);
    }
    
    sample->AppendByte(static_cast<uint8_t>(buffer[0]) & mask);

    return sample;
}

SharePtrSample SampleReader::NextBytes(size_t length)
{
    auto sample = std::make_shared<Sample>();

    char buffer[ChunkSize] = {0};
    auto pbuf = reinterpret_cast<const uint8_t*>(buffer);

    while (length > 0) {
        auto read = ReadChunk(buffer);
        if (read > length) {
            read = length;
        }

        sample->AppendBytes(pbuf, read);
        length -= read;
    }

    return sample;
}

size_t SampleReader::ReadChunk(char* buffer)
{
    auto curr = ifs.tellg();
    ifs.seekg(0, std::ios::end);
    auto length = ifs.tellg() - curr;
    
    ifs.seekg(curr, std::ios::beg);

    if (length > ChunkSize) {
        length = ChunkSize;
    }

    ifs.read(buffer, length);

    return length;
}