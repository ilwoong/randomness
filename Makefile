CC = g++
CPPFLAGS = -O2 -fopenmp

SRC_SP800_22 = \
	common/sample.cpp \
	common/sample_reader.cpp \
	common/hamming_weight.cpp \
	algorithm/numerical_recipes.cpp \
	sp800-22/monobit_test.cpp \
	sp800-22/block_frequency_test.cpp \
	sp800-22/runs_test.cpp \
	sp800-22/longest_run_test.cpp \

SRC_SP800_90B = \
	algorithm/lcp_array.cpp \
	algorithm/suffix_array.cpp \
	sp800-90b/estimator/binary_search.cpp \
	sp800-90b/estimator/entropy_estimator.cpp \
	sp800-90b/estimator/mcv_estimator.cpp \
	sp800-90b/estimator/collision_estimator.cpp \
	sp800-90b/estimator/markov_estimator.cpp \
	sp800-90b/estimator/compression_estimator.cpp \
	sp800-90b/estimator/tuple_estimator.cpp \
	sp800-90b/estimator/lrs_estimator.cpp \
	sp800-90b/estimator/prediction_evaluator.cpp \
	sp800-90b/estimator/prediction_estimator.cpp \
	sp800-90b/estimator/mcw_predictor.cpp \
	sp800-90b/estimator/multi_mcw_prediction_estimator.cpp \
	sp800-90b/estimator/lag_prediction_estimator.cpp \
	sp800-90b/estimator/mmc_predictor.cpp \
	sp800-90b/estimator/multi_mmc_prediction_estimator.cpp \
	sp800-90b/estimator/lz78y_predictor.cpp \
	sp800-90b/estimator/lz78y_prediction_estimator.cpp \

.PHONY: all clean

all: sts ea_non_iid

sts: test/sts.cpp $(SRC_SP800_22)
	$(CC) $(CPPFLAGS) $^ -o $@

ea_non_iid: test/ea_non_iid.cpp $(SRC_SP800_90B)
	$(CC) $(CPPFLAGS) $^ -o $@

clean:
	rm -rf sts ea_non_iid