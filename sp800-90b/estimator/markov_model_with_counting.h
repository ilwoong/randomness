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

#ifndef __RANDOMNESS_SP800_90B_ESTIMATOR_PREDICTION_MARKOV_MODEL_WITH_COUNTING_H__
#define __RANDOMNESS_SP800_90B_ESTIMATOR_PREDICTION_MARKOV_MODEL_WITH_COUNTING_H__

#include <array>
#include <cstdint>
#include <map>
#include <vector>

#include "mcv_tracker.h"

namespace randomness { namespace sp800_90b { namespace estimator {

    static constexpr size_t MaxEntries = 100000;
    using trace_t = std::vector<uint8_t>;
    using counter_t = std::array<size_t, 256>;
    using chain_t = std::map<trace_t, McvTracker>;

    class MarkovModelWithCounting {

    private:
        size_t order;
        size_t entries;
        trace_t trace;
        chain_t chain;

    public:
        void init(const uint8_t* sample, size_t order);
        void new_entry(uint8_t sample);
        int16_t update_entry(chain_t::iterator& iter, uint8_t sample);
        int16_t predict(uint8_t sample);

    private:
        void update_trace(uint8_t sample);
    };

    using binary_trace_t = int16_t;
    using binary_chain_t = std::map<uint16_t, McvTracker>;

    class MarkovModelWithCountingBinary {
    private:
        size_t order;
        size_t entries;
        binary_trace_t mask;
        binary_trace_t trace;
        binary_chain_t chain;

    public:
        void init(const uint8_t* sample, size_t order);
        void new_entry(uint8_t sample);
        int16_t update_entry(binary_chain_t::iterator& iter, uint8_t sample);
        int16_t predict(uint8_t sample);

    private:
        void update_trace(uint8_t sample);
    };

}}}

#endif