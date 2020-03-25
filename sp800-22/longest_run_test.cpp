#include "longest_run_test.h"

#include "../algorithm/numerical_recipes.h"

using namespace randomness::algorithm;
using namespace randomness::sp800_22;

static constexpr double PI3[] = { 0.2148, 0.3672, 0.2305, 0.1875 };
static constexpr double PI5[] = { 0.1174, 0.2430, 0.2493, 0.1752, 0.1027, 0.1124 };
static constexpr double PI6[] = { 0.0882, 0.2092, 0.2483, 0.1933, 0.1208, 0.0675, 0.0727 };

static constexpr size_t RANGE3[] = { 1, 2, 3, 4 };
static constexpr size_t RANGE5[] = { 4, 5, 6, 7, 8, 9 };
static constexpr size_t RANGE6[] = { 10, 11, 12, 13, 14, 15, 16 };

const std::string LongestRunTest::Name() const
{
    return "Longest run of ones in a block test";
}

const std::string LongestRunTest::ShortName() const
{
    return "Longest Run";
}

size_t LongestRunTest::MinimumLengthInBits() const 
{
    return 128;
}

static size_t FindLongestRun(const uint8_t* data, size_t offset, size_t blockLength)
{
    size_t longest = 0;
    size_t run = 0;
    
    data += offset;
    for (size_t i = 0; i < blockLength; ++i) {
        if (*(data++) == 1) {
            run += 1;
        } 
        else {
            if (longest < run) {
                longest = run;
            }

            run = 0;
        }
    }

    return longest;
}

std::vector<randomness_result_t> LongestRunTest::Evaluate(const Sample& sample)
{
    Initialize(sample.BinaryData().size());
    
    auto chisquare = CalculateStatistic(sample);
    auto pvalue = igammac(dof/2.0, chisquare/2.0);

    auto result = std::vector<randomness_result_t>{};
    result.push_back(randomness_result_t {Name(), ShortName(), "", pvalue});

    logstream << "countBlocks = " << countBlocks << ", 𝛘² = " << chisquare;
    
    return result;
}

void LongestRunTest::Initialize(size_t length)
{
    blockLength = 8;
    dof = 3;
    pi = PI3;
    range = RANGE3;

    if (length >= 6272) {
        blockLength = 128;
        dof = 5;
        pi = PI5;
        range = RANGE5;
    }

    if (length >= 750000) {
        blockLength = 10000;
        dof = 6;
        pi = PI6;
        range = RANGE6;
    }

    countBlocks = length / blockLength;
    frequencies.assign(dof + 1, 0);
}

double LongestRunTest::CalculateStatistic(const Sample& sample)
{
    auto sum = 0.0;
    auto data = sample.BinaryData();

    for (auto i = 0; i < data.size(); i += blockLength) {
        auto longest = FindLongestRun(data.data(), i, blockLength);
        UpdateFrequencies(longest);
    }

    for (auto i = 0; i <= dof; ++i) {
        auto term = frequencies[i] - countBlocks * pi[i];
        sum += term * term / (countBlocks * pi[i]);
    }

    return sum;
}

void LongestRunTest::UpdateFrequencies(size_t longest)
{
    if (longest <= range[0]) {
        frequencies[0] += 1;
        return;
    }

    for (auto k = 1; k < dof; ++k) {
        if (longest == range[k]) {
            frequencies[k] += 1;
            return;
        }
    }

    if (longest >= range[dof]) {
        frequencies[dof] += 1;
    }
}