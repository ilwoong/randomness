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

#include "../sp800-90b/estimator/mcv_estimator.h"
#include "../sp800-90b/estimator/tuple_estimator.h"
#include "../sp800-90b/estimator/lrs_estimator.h"

#include <array>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <vector>

#include <omp.h>

using namespace randomness::sp800_90b;
using namespace randomness::sp800_90b::estimator;

static constexpr size_t MILLION = 1000000;

std::streamsize read_bytes(char* data, const char* filepath, std::streamsize length)
{
    std::ifstream ifs;    
    ifs.open(filepath, std::ios::binary);
    if (ifs.good() == false) {
        std::cout << "file open failed" << std::endl;
    }

    ifs.seekg(0, std::ios::end);
    auto size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    if (size < length) {
        length = size;
    }

    ifs.read(data, length);

    auto ret = ifs.gcount();
    ifs.close();
    
    return ret;
}

void write_bytes(const char* filepath, const char* data, size_t length)
{
    std::ofstream ofs;
    ofs.open(filepath, std::ios::binary);

    ofs.write(data, length);
    ofs.close();
}

std::vector<std::shared_ptr<EntropyEstimator>> get_estimators()
{
    std::vector<std::shared_ptr<EntropyEstimator>> estimators;
    estimators.push_back(std::make_shared<McvEstimator>());
    estimators.push_back(std::make_shared<TupleEstimator>());
    estimators.push_back(std::make_shared<LrsEstimator>());

    return estimators;
}

void run_estimators(const char* filepath) 
{    
    std::vector<uint8_t> data(MILLION, 0);
    auto read = read_bytes(reinterpret_cast<char*>(data.data()), filepath, MILLION);
    
    std::vector<uint8_t> bin_data;
    for (size_t i = 0; i < 1000000; ++i) {
        uint8_t letter = data[i];
        for (int i = 0; i < 8; ++i) {
            bin_data.push_back((letter & 0x80) >> 7);
            letter <<= 1;
        }
    }

    /* binary */
    auto pdata = bin_data.data();
    auto data_length = bin_data.size();
    auto alph_size = 2;
    /**/

    /* literal * /
    auto pdata = data.data();
    auto data_length = data.size();
    auto alph_size = 256;
    /**/

    std::cout << read << " bytes are read" << std::endl;

    auto estimators = get_estimators();
    double total_elapsed = omp_get_wtime();

#pragma omp parallel for
    for (int i = 0; i < estimators.size(); ++i) {
        auto estimator = estimators[i];
        auto elapsed = omp_get_wtime();
        double entropy = estimator->Estimate(pdata, data_length, alph_size);
        elapsed = omp_get_wtime() - elapsed;

        std::cout << std::setw(30) << std::fixed << std::right << estimator->Name() << ": ";
        std::cout << entropy << ", elapsed: ";
        std::cout << elapsed << std::endl;

        std::cout << std::setw(30) << std::fixed << std::right << estimator->Name() << ": ";
        std::cout << estimator->Log() << std::endl;
    }
    total_elapsed = omp_get_wtime() - total_elapsed;
    std::cout << "\nElapsed total: " << total_elapsed << " seconds." << std::endl;
}

int main(int argc, const char** argv)
{
    run_estimators("./samples/random_1MB.bin");
    return 0;
}