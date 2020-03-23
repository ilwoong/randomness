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

#ifndef __RANDOMNESS_SP800_22_STATISTICAL_TEST_H__
#define __RANDOMNESS_SP800_22_STATISTICAL_TEST_H__

#include <cmath>
#include <string>
#include <sstream>
#include <vector>

#include "../common/sample.h"

namespace randomness { namespace sp800_22 {

    using namespace randomness::common;

    typedef struct {
        std::string name;
        std::string shortname;
        std::string param;
        double pvalue;
    } randomness_result_t;

    class StatisticalTest 
    {
    protected:
        std::ostringstream logstream;
    
    public:
        std::string Log() const {
            return logstream.str();
        }

        virtual const std::string Name() const = 0;
        virtual const std::string ShortName() const = 0;
        virtual std::vector<randomness_result_t> Evaluate(const Sample& sample) = 0;
    };
}}

#endif
