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

#ifndef __RANDOMNESS_SP800_90B_ESTIMATOR_MCV_TRACKER_H__
#define __RANDOMNESS_SP800_90B_ESTIMATOR_MCV_TRACKER_H__

#include <array>
#include <cstdint>

namespace randomness { namespace sp800_90b { namespace estimator {

    typedef struct {
        int16_t key = -1;
        size_t count = 0;
    } mcv_info_t;

    class McvTracker {
    private:
        mcv_info_t mcv;
        std::array<size_t, 256> counter = { 0, };

    public:
        void Create(uint8_t feed)
        {
            counter[feed] = 1;
            mcv.key = feed;
            mcv.count = 1;
        }

        void Update(uint8_t feed) 
        {
            counter[feed] += 1;
            if ((mcv.count < counter[feed]) || (mcv.count == counter[feed]) && (feed > mcv.key)) {
                mcv.key = feed;
                mcv.count = counter[feed];
            }
        }

        mcv_info_t MostCommonValue() const
        {
            return mcv;
        }

        int16_t MostCommonKey() const 
        {
            return mcv.key;
        }

        size_t MostCommonCount() const
        {
            return mcv.count;
        }
    };
}}}

#endif