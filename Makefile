CC = g++
CPPFLAGS = -O2 -fopenmp

SRC_SP800_22 = \
	common/sample.cpp \
	common/sample_reader.cpp \
	sp800-22/monobit_test.cpp \

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
	sp800-90b/estimator/prediction_estimator.cpp \
	sp800-90b/estimator/scoreboard_estimator.cpp \
	sp800-90b/estimator/multi_mcw_prediction_estimator.cpp \
	sp800-90b/estimator/lag_prediction_estimator.cpp \

.PHONY: all clean

all: sts ea_non_iid

sts: test/sts.cpp $(SRC_SP800_22)
	$(CC) $(CPPFLAGS) $^ -o $@

ea_non_iid: test/ea_non_iid.cpp $(SRC_SP800_90B)
	$(CC) $(CPPFLAGS) $^ -o $@

clean:
	rm -rf sts ea_non_iid