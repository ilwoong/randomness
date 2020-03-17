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

#include "binary_search.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

using namespace randomness::sp800_90b::estimator;

static constexpr double Epsilon = 4 * std::numeric_limits<double>::epsilon();
static constexpr size_t MaxIterations = 1076;

static bool IsOutOpenInterval(double value, double itv1, double itv2)
{
    double low = std::min(itv1, itv2);
    double high = std::max(itv1, itv2);

    return (value <= low) || (value >= high);
}

static bool IsOutClosedInterval(double value, double itv1, double itv2)
{
    double low = std::min(itv1, itv2);
    double high = std::max(itv1, itv2);

    return (value < low) || (value > high);
}

BinarySearch::BinarySearch(std::function<double(double, double)> func)
{
    SetFunction(func);
}

void BinarySearch::SetFunction(std::function<double(double, double)> func) 
{
    evaluate = func;
}

double BinarySearch::FindSolution(double arg, double ldomain, double hdomain, double optional)
{
    if (arg >= evaluate(ldomain, optional)) {
        throw std::invalid_argument("invalid argument for binary search");
    }

    double lbound = ldomain;
    double hbound = hdomain;

    double lvalue = std::numeric_limits<double>::infinity();
    double hvalue = -std::numeric_limits<double>::infinity();

    double p = (lbound + hbound) / 2.0;
    double prev = 0;
    double eval = evaluate(p, optional);

    for (size_t i = 0; i < MaxIterations; ++i) {
        if (fabs(eval - arg) < Epsilon) {
            break;
        }

        if (arg < eval) {
            lbound = p;
            lvalue = eval;
        }
        else {
            hbound = p;
            hvalue = eval;
        }

        if (lbound >= hbound) {        
            p = std::min(std::max(lbound, hbound), hdomain);
            break;
        }

        if (IsOutClosedInterval(lbound, ldomain, hdomain) || IsOutClosedInterval(hbound, ldomain, hdomain)) {
            p = ldomain;
            break;
        }

        if (IsOutClosedInterval(arg, lvalue, hvalue)) {    
            p = ldomain;
            break;
        }
        
        prev = p;
        p = (lbound + hbound) / 2.0;

        if (IsOutOpenInterval(p, lbound, hbound)) {
            p = hbound;
            break;
        }

        if (prev == p) {
            p = hbound;
            break;
        }

        eval = evaluate(p, optional);

        if (IsOutClosedInterval(eval, lvalue, hvalue)) {
            p = hbound;
            break;
        }
    }

    if (p <= ldomain) {
        throw std::underflow_error("binary search failed");
    }

    return p;
}