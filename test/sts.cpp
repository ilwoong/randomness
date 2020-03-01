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

#include <iostream>

#include "../common/sample_reader.h"
#include "../sp800-22/monobit_test.h"

using namespace randomness::common;
using namespace randomness::sp800_22;

std::vector<std::shared_ptr<StatisticalTest>> GetTests()
{
    auto tests = std::vector<std::shared_ptr<StatisticalTest>>();
    tests.push_back(std::make_shared<MonobitTest>());

    return tests;
}

int main(int argc, const char** argv)
{
    SampleReader reader;
    reader.Open("./samples/random_1MB.bin");
    auto sample = reader.NextBits(1000000);

    auto tests = GetTests();    
    for (auto test : tests){
        test->Run(*sample);
        std::cout << test->Name() << ": " << test->PValues()[0] << std::endl;
        std::cout << test->Name() << ": " << test->Log() << std::endl;
    }

    reader.Close();

    return 0;
}