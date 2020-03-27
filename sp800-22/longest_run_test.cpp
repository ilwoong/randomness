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
    if (length < 128) {
        throw std::invalid_argument("sample length is too short");
    }
    else if (length < 6272) {
        Initialize(8, 3, PI3, RANGE3);
    }
    else if (length < 750000) {
        Initialize(128, 5, PI5, RANGE5);
    }
    else {
        Initialize(10000, 6, PI6, RANGE6);
    }

    countBlocks = length / blockLength;
    frequencies.assign(dof + 1, 0);
}

void LongestRunTest::Initialize(size_t blockLength, size_t dof, const double* pi, const size_t* range)
{
    this->blockLength = blockLength;
    this->dof = dof;
    this->pi = pi;
    this->range = range;
}

double LongestRunTest::CalculateStatistic(const Sample& sample)
{
    BuildFrequencyTable(sample.BinaryData().data());

    auto sum = 0.0;
    for (size_t i = 0; i <= dof; ++i) {
        auto term = frequencies[i] - countBlocks * pi[i];
        sum += term * term / (countBlocks * pi[i]);
    }

    return sum;
}

void LongestRunTest::BuildFrequencyTable(const uint8_t* block)
{
    for (auto i = 0; i < countBlocks; ++i, block += blockLength) {
        auto longest = FindLongestRun(block);
        UpdateFrequencies(longest);
    }
}

size_t LongestRunTest::FindLongestRun(const uint8_t* block)
{
    size_t longest = 0;
    size_t run = 0;
    
    for (size_t i = 0; i < blockLength; ++i) {
        if (*(block++) == 1) {
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

void LongestRunTest::UpdateFrequencies(size_t longest)
{
    if (longest <= range[0]) {
        frequencies[0] += 1;
        return;
    }
    else if (longest >= range[dof]) {
        frequencies[dof] += 1;
        return;
    }

    frequencies[longest - range[0]] += 1;
}