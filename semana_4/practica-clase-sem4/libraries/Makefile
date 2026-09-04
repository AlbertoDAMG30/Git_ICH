CC := gcc
BUILD_DIR := build
LIB_DIR := $(BUILD_DIR)/lib
BIN_DIR := $(BUILD_DIR)/bin

BASE_CFLAGS := -Wall -Wextra -O2
PIC_CFLAGS := $(BASE_CFLAGS) -fPIC
LDFLAGS_DYNAMIC := -L$(LIB_DIR) -Wl,-rpath,'$$ORIGIN/../lib'
ARGS ?= 1000000 1000 1.0 2.0

STATIC_LIB := $(LIB_DIR)/libvectorops.a
DYNAMIC_LIB := $(LIB_DIR)/libvectorops.so

TARGETS := \
	$(BIN_DIR)/bench-static \
	$(BIN_DIR)/bench-dynamic \
	$(BIN_DIR)/bench-inline \
	$(BIN_DIR)/bench-static-lto

.PHONY: all run run-all size symbols clean

all: $(TARGETS)

run: $(BIN_DIR)/bench-static
	./$< $(ARGS)

run-all: all
	@for bin in $(TARGETS); do \
		echo; \
		echo "== $$bin =="; \
		./$$bin $(ARGS); \
	done

size: all
	ls -lh $(STATIC_LIB) $(DYNAMIC_LIB) $(TARGETS)
	size $(STATIC_LIB) $(DYNAMIC_LIB) $(TARGETS)

symbols: all
	nm -C $(STATIC_LIB)
	nm -D -C $(DYNAMIC_LIB)

$(BUILD_DIR) $(LIB_DIR) $(BIN_DIR):
	mkdir -p $@

$(BUILD_DIR)/vector_ops.o: vector_ops.c vector_ops.h | $(BUILD_DIR)
	$(CC) $(BASE_CFLAGS) -c $< -o $@

$(BUILD_DIR)/vector_ops.pic.o: vector_ops.c vector_ops.h | $(BUILD_DIR)
	$(CC) $(PIC_CFLAGS) -c $< -o $@

$(BUILD_DIR)/benchmark_common.o: benchmark_common.c benchmark_common.h | $(BUILD_DIR)
	$(CC) $(BASE_CFLAGS) -c $< -o $@

$(BUILD_DIR)/benchmark_common.lto.o: benchmark_common.c benchmark_common.h | $(BUILD_DIR)
	$(CC) $(BASE_CFLAGS) -flto -c $< -o $@

$(BUILD_DIR)/vector_ops.lto.o: vector_ops.c vector_ops.h | $(BUILD_DIR)
	$(CC) $(BASE_CFLAGS) -flto -c $< -o $@

$(STATIC_LIB): $(BUILD_DIR)/vector_ops.o | $(LIB_DIR)
	ar rcs $@ $<

$(DYNAMIC_LIB): $(BUILD_DIR)/vector_ops.pic.o | $(LIB_DIR)
	$(CC) -shared $< -o $@

$(BIN_DIR)/bench-static: benchmark_library.c benchmark_common.h vector_ops.h $(STATIC_LIB) $(BUILD_DIR)/benchmark_common.o | $(BIN_DIR)
	$(CC) $(BASE_CFLAGS) benchmark_library.c $(BUILD_DIR)/benchmark_common.o $(STATIC_LIB) -o $@

$(BIN_DIR)/bench-dynamic: benchmark_library.c benchmark_common.h vector_ops.h $(DYNAMIC_LIB) $(BUILD_DIR)/benchmark_common.o | $(BIN_DIR)
	$(CC) $(BASE_CFLAGS) benchmark_library.c $(BUILD_DIR)/benchmark_common.o $(LDFLAGS_DYNAMIC) -lvectorops -o $@

$(BIN_DIR)/bench-inline: benchmark_inline.c benchmark_common.h vector_ops_inline.h $(BUILD_DIR)/benchmark_common.o | $(BIN_DIR)
	$(CC) $(BASE_CFLAGS) benchmark_inline.c $(BUILD_DIR)/benchmark_common.o -o $@

$(BIN_DIR)/bench-static-lto: benchmark_library.c benchmark_common.h vector_ops.h $(BUILD_DIR)/benchmark_common.lto.o $(BUILD_DIR)/vector_ops.lto.o | $(BIN_DIR)
	$(CC) $(BASE_CFLAGS) -flto benchmark_library.c $(BUILD_DIR)/benchmark_common.lto.o $(BUILD_DIR)/vector_ops.lto.o -o $@

clean:
	rm -rf $(BUILD_DIR)
