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

#ifndef __RANDOMNESS_SP800_90B_ESTIMATOR_LZ78Y_PREDICTOR_H__
#define __RANDOMNESS_SP800_90B_ESTIMATOR_LZ78Y_PREDICTOR_H__

#include <cstdint>

#include <map>
#include <vector>

#include "mcv_tracker.h"

namespace randomness { namespace sp800_90b { namespace estimator {

    class Lz78yPredictor {
    public:
        virtual void Initialize(const uint8_t* sample, size_t order) = 0;
        virtual mcv_info_t Predict(uint8_t sample) = 0;
        virtual void CreateEntry(uint8_t sample) = 0;
        virtual void UpdateTrace(uint8_t sample) = 0;
    };

    class Lz78yPredictorBinary : public Lz78yPredictor 
    {
    using trace_t = uint16_t;
    using dict_t = std::vector<size_t>;

    private:
        trace_t mask;
        trace_t trace;
        dict_t dictionary;

    public:
        void Initialize(const uint8_t* sample, size_t order) override;
        mcv_info_t Predict(uint8_t sample) override;
        void CreateEntry(uint8_t sample) override;
        void UpdateTrace(uint8_t sample) override;
    };

    class Lz78yPredictorLiteral : public Lz78yPredictor 
    {
    using trace_t = std::vector<uint8_t>;
    using dict_t = std::map<trace_t, McvTracker>;

    private:
        trace_t trace;
        dict_t dictionary;

    public:
        void Initialize(const uint8_t* sample, size_t order) override;
        mcv_info_t Predict(uint8_t sample) override;
        void CreateEntry(uint8_t sample) override;
        void UpdateTrace(uint8_t sample) override;
    };

}}}

#endif