# Makefile for low_latency_lob_sim
# Default: configure + build executable

BUILD_DIR ?= build
BIN ?= $(BUILD_DIR)/lob_bench.tsk
ORDERS ?= 5000000
SEED ?= 42
PERF_FREQ ?= 999

.PHONY: all configure build rebuild run profile perf-record perf-report flame bench clean clean-perf

all: build

configure:
	cmake -S . -B $(BUILD_DIR) -DPERF_BUILD=ON

build: configure
	cmake --build $(BUILD_DIR) -j

rebuild:
	rm -rf $(BUILD_DIR)
	$(MAKE) build

run: build
	./$(BIN) $(ORDERS) $(SEED)

profile: build
	./$(BIN) $(ORDERS) $(SEED) --profile-mode

perf-record: build
	perf record -e cycles:u -F $(PERF_FREQ) -g -- ./$(BIN) $(ORDERS) $(SEED) --profile-mode

perf-report:
	perf report --stdio -n --no-children | head -n 80

flame: perf-record
	perf script > out.perf
	../..//FlameGraph/stackcollapse-perf.pl out.perf > out.folded
	../..//./FlameGraph/flamegraph.pl --width 2400 --minwidth 0.1 --fontsize 10 out.folded > flame.svg
	@echo "Generated flame.svg"

bench: build
	chmod +x benchmark.sh
	./benchmark.sh

clean:
	rm -rf $(BUILD_DIR)

clean-perf:
	rm -f perf.data perf.data.old out.perf out.folded flame.svg
