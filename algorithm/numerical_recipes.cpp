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

#include "numerical_recipes.h"

#include <cmath>
#include <stdexcept>

static const double MACHEP = 1.11022302462515654042E-16;		// 2**-53
static const double MAXNUM = 1.7976931348623158E308;		// 2**1024*(1-MACHEP)
static const double MAXLOG = 7.09782712893383996732224E2;	// log(MAXNUM)

static double BIG = 4.503599627370496e15;
static double BIGINV = 2.22044604925031308085e-16;

using namespace randomness::algorithm;

double randomness::algorithm::igamma(double x, double a)
{
    if ((x <= 0.0) || (a <= 0.0)) {
        return 0.0;
    }

    if (x >= 1.0 && x > a) {
        return 1.e0 - igammac(a, x);
    }

    double ax = a * log(x) - x - std::lgamma(a);
    if (ax < -MAXLOG) {
        return 0.0;
    }
    ax = exp(ax);

    // power series
    double r = a;
    double c = 1.0;
    double ans = 1.0;

    do {
        r += 1.0;
        c *= x / r;
        ans += c;
    } while (c / ans > MACHEP);

    return ans * ax / a;
}

double randomness::algorithm::igammac(double x, double a)
{
    double ans, ax, c, yc, r, t, y, z;
    double pk, pkm1, pkm2, qk, qkm1, qkm2;

    if ((x <= 0) || (a <= 0)) {
        return 1.0;
    }

    if ((x < 1.0) || (x < a)) {
        return 1.e0 - igamma(a, x);
    }

    ax = a * log(x) - x - std::lgamma(a);

    if (ax < -MAXLOG) {
        return 1.0;
    }
    ax = exp(ax);

    /* continued fraction */
    y = 1.0 - a;
    z = x + y + 1.0;
    c = 0.0;
    pkm2 = 1.0;
    qkm2 = x;
    pkm1 = x + 1.0;
    qkm1 = z * x;
    ans = pkm1 / qkm1;

    do {
        c += 1.0;
        y += 1.0;
        z += 2.0;
        yc = y * c;
        pk = pkm1 * z - pkm2 * yc;
        qk = qkm1 * z - qkm2 * yc;
        if (qk != 0) {
            r = pk / qk;
            t = fabs((ans - r) / r);
            ans = r;
        }
        else {
            t = 1.0;
        }

        pkm2 = pkm1;
        pkm1 = pk;
        qkm2 = qkm1;
        qkm1 = qk;
        if (fabs(pk) > BIG) {
            pkm2 *= BIGINV;
            pkm1 *= BIGINV;
            qkm2 *= BIGINV;
            qkm1 *= BIGINV;
        }
    } while (t > MACHEP);

    return ans * ax;
}