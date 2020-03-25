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

#include <iomanip>
#include <iostream>
#include <sstream>

#include "../common/sample_reader.h"
#include "../sp800-22/evaluators.h"

using namespace randomness::common;
using namespace randomness::sp800_22;

static constexpr size_t TitleWidth = 20;

std::vector<std::shared_ptr<StatisticalTest>> PopulateTests()
{
    auto tests = std::vector<std::shared_ptr<StatisticalTest>>();
    tests.push_back(std::make_shared<MonobitTest>());
    tests.push_back(std::make_shared<BlockFrequencyTest>());
    tests.push_back(std::make_shared<RunsTest>());
    tests.push_back(std::make_shared<LongestRunTest>());

    return tests;
}

static void PrintResultItem(std::vector<randomness_result_t> results, std::string log) 
{
    for (auto item : results) {
        std::ostringstream oss;
        oss << item.shortname;
        if (item.param.length() > 0) {
            oss << " ("<< item.param << ")";
        }

        std::cout << std::setw(TitleWidth) << std::fixed << std::right << oss.str();
        std::cout << ": P-value = " << item.pvalue << std::endl;

        std::cout << std::setw(TitleWidth) << std::fixed << std::right << oss.str();
        std::cout << ": " << log << std::endl;
    }
}

int main(int argc, const char** argv)
{
    SampleReader reader;
    reader.Open("./samples/random_1MB.bin");
    auto sample = reader.NextBits(1000000);

    auto tests = PopulateTests();
    for (auto test : tests){
        auto results = test->Evaluate(*sample);
        PrintResultItem(results, test->Log());
    }

    reader.Close();

    return 0;
}