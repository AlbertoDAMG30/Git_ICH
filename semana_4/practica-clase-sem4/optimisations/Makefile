CC := gcc
SRC := vector-add.c
BUILD_DIR := build
PGO_DIR := $(BUILD_DIR)/pgo

BASE_CFLAGS := -Wall -Wextra -fopenmp
ARGS ?=
PGO_ARGS ?= $(ARGS)

TARGETS := \
	$(BUILD_DIR)/vector-add-O0 \
	$(BUILD_DIR)/vector-add-O1 \
	$(BUILD_DIR)/vector-add-O2 \
	$(BUILD_DIR)/vector-add-O3 \
	$(BUILD_DIR)/vector-add-Ofast \
	$(BUILD_DIR)/vector-add-Os \
	$(BUILD_DIR)/vector-add-Og \
	$(BUILD_DIR)/vector-add-inline \
	$(BUILD_DIR)/vector-add-no-inline \
	$(BUILD_DIR)/vector-add-march-native \
	$(BUILD_DIR)/vector-add-vectorize \
	$(BUILD_DIR)/vector-add-no-vectorize \
	$(BUILD_DIR)/vector-add-lto

.PHONY: all run run-all pgo vector-report clean

all: $(TARGETS)

run: $(BUILD_DIR)/vector-add-O2
	./$< $(ARGS)

run-all: all
	@for bin in $(TARGETS); do \
		echo; \
		echo "== $$bin =="; \
		./$$bin $(ARGS); \
	done

pgo: $(BUILD_DIR)/vector-add-pgo

$(BUILD_DIR):
	mkdir -p $@

$(PGO_DIR): | $(BUILD_DIR)
	mkdir -p $@

$(BUILD_DIR)/vector-add-O0: CFLAGS_OPT := -O0
$(BUILD_DIR)/vector-add-O1: CFLAGS_OPT := -O1
$(BUILD_DIR)/vector-add-O2: CFLAGS_OPT := -O2
$(BUILD_DIR)/vector-add-O3: CFLAGS_OPT := -O3
$(BUILD_DIR)/vector-add-Ofast: CFLAGS_OPT := -Ofast
$(BUILD_DIR)/vector-add-Os: CFLAGS_OPT := -Os
$(BUILD_DIR)/vector-add-Og: CFLAGS_OPT := -Og
$(BUILD_DIR)/vector-add-inline: CFLAGS_OPT := -O3 -finline-functions -finline-limit=10000
$(BUILD_DIR)/vector-add-no-inline: CFLAGS_OPT := -O3 -fno-inline -fno-inline-functions
$(BUILD_DIR)/vector-add-march-native: CFLAGS_OPT := -O3 -march=native -mtune=native
$(BUILD_DIR)/vector-add-vectorize: CFLAGS_OPT := -O3 -ftree-vectorize
$(BUILD_DIR)/vector-add-no-vectorize: CFLAGS_OPT := -O3 -fno-tree-vectorize
$(BUILD_DIR)/vector-add-lto: CFLAGS_OPT := -O3 -flto

$(TARGETS): $(SRC) | $(BUILD_DIR)
	$(CC) $(BASE_CFLAGS) $(CFLAGS_OPT) $< -o $@

$(BUILD_DIR)/vector-add-pgo: $(SRC) | $(BUILD_DIR) $(PGO_DIR)
	rm -rf $(PGO_DIR)
	mkdir -p $(PGO_DIR)
	$(CC) $(BASE_CFLAGS) -O3 -march=native -fprofile-generate=$(PGO_DIR) $< -o $@
	./$@ $(PGO_ARGS)
	$(CC) $(BASE_CFLAGS) -O3 -march=native -fprofile-use=$(PGO_DIR) -fprofile-correction $< -o $@

vector-report: $(SRC) | $(BUILD_DIR)
	$(CC) $(BASE_CFLAGS) -O3 -march=native -ftree-vectorize \
		-fopt-info-vec-optimized=$(BUILD_DIR)/vectorization-report.txt \
		$< -o $(BUILD_DIR)/vector-add-vector-report
	@cat $(BUILD_DIR)/vectorization-report.txt

clean:
	rm -rf $(BUILD_DIR)
