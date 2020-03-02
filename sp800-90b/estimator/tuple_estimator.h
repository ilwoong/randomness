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

#ifndef _RANDOMNESS_SP800_90B_ESTIMATOR_TUPLE_H__
#define _RANDOMNESS_SP800_90B_ESTIMATOR_TUPLE_H__

#include "../entropy_estimator.h"
#include "../../algorithm/lcp_array.h"

#include <vector>

namespace randomness { namespace sp800_90b { namespace estimator {

    using namespace randomness::algorithm;

    class TupleEstimator : public EntropyEstimator 
    {
    public:
        std::string Name() const override;
        double Estimate(const uint8_t* data, size_t length, size_t count_alphabets) override;
        virtual double Estimate(const LcpArray& lcp);

    protected:
        std::vector<size_t> GetMaximumTupleCounts(const LcpArray& lcp, size_t length);

    private:
        double CalculateMaximumProbability(const std::vector<size_t>& Q, size_t t, size_t length) const;
    };
}}}

#endif