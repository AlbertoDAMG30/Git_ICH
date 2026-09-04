CC := gcc
SRC := popcount-naive.c
BUILD_DIR := build
PGO_DIR := $(BUILD_DIR)/pgo

BASE_CFLAGS := -Wall -Wextra -fopenmp
ARGS ?=
PGO_ARGS ?= $(ARGS)

TARGETS := \
	$(BUILD_DIR)/popcount-naive-O0 \
	$(BUILD_DIR)/popcount-naive-O1 \
	$(BUILD_DIR)/popcount-naive-O2 \
	$(BUILD_DIR)/popcount-naive-O3 \
	$(BUILD_DIR)/popcount-naive-Ofast \
	$(BUILD_DIR)/popcount-naive-Os \
	$(BUILD_DIR)/popcount-naive-Og \
	$(BUILD_DIR)/popcount-naive-inline \
	$(BUILD_DIR)/popcount-naive-no-inline \
	$(BUILD_DIR)/popcount-naive-march-native \
	$(BUILD_DIR)/popcount-naive-lto

.PHONY: all run run-all pgo clean

all: $(TARGETS)

run: $(BUILD_DIR)/popcount-naive-O2
	./$< $(ARGS)

run-all: all
	@for bin in $(TARGETS); do \
		echo; \
		echo "== $$bin =="; \
		./$$bin $(ARGS); \
	done

pgo: $(BUILD_DIR)/popcount-naive-pgo

$(BUILD_DIR):
	mkdir -p $@

$(PGO_DIR): | $(BUILD_DIR)
	mkdir -p $@

$(BUILD_DIR)/popcount-naive-O0: CFLAGS_OPT := -O0
$(BUILD_DIR)/popcount-naive-O1: CFLAGS_OPT := -O1
$(BUILD_DIR)/popcount-naive-O2: CFLAGS_OPT := -O2
$(BUILD_DIR)/popcount-naive-O3: CFLAGS_OPT := -O3
$(BUILD_DIR)/popcount-naive-Ofast: CFLAGS_OPT := -Ofast
$(BUILD_DIR)/popcount-naive-Os: CFLAGS_OPT := -Os
$(BUILD_DIR)/popcount-naive-Og: CFLAGS_OPT := -Og
$(BUILD_DIR)/popcount-naive-inline: CFLAGS_OPT := -O3 -finline-functions -finline-limit=10000
$(BUILD_DIR)/popcount-naive-no-inline: CFLAGS_OPT := -O3 -fno-inline -fno-inline-functions
$(BUILD_DIR)/popcount-naive-march-native: CFLAGS_OPT := -O3 -march=native -mtune=native
$(BUILD_DIR)/popcount-naive-lto: CFLAGS_OPT := -O3 -flto

$(TARGETS): $(SRC) | $(BUILD_DIR)
	$(CC) $(BASE_CFLAGS) $(CFLAGS_OPT) $< -o $@

$(BUILD_DIR)/popcount-naive-pgo: $(SRC) | $(BUILD_DIR) $(PGO_DIR)
	rm -rf $(PGO_DIR)
	mkdir -p $(PGO_DIR)
	$(CC) $(BASE_CFLAGS) -O3 -march=native -fprofile-generate=$(PGO_DIR) $< -o $@
	./$@ $(PGO_ARGS)
	$(CC) $(BASE_CFLAGS) -O3 -march=native -fprofile-use=$(PGO_DIR) -fprofile-correction $< -o $@

clean:
	rm -rf $(BUILD_DIR)
