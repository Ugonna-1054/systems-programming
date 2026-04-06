#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN_PATH="${ROOT_DIR}/lob_bench"
CSV_PATH="${ROOT_DIR}/benchmark_results.csv"

SEEDS=(42 43 44 45 46)

workloads=(
  "small 100000"
  # "mid 1000000"
  # "large 5000000"
  # "xlarge 20000000"
)

build() {
  clang++ -std=c++20 -O3 -march=native -Wall -Wextra -Wpedantic \
    -I"${ROOT_DIR}/include" \
    "${ROOT_DIR}/src/main.cpp" "${ROOT_DIR}/src/order_book.cpp" \
    -o "${BIN_PATH}"
}

init_csv() {
  cat > "${CSV_PATH}" <<CSV
workload,run_type,run_idx,orders,seed,elapsed_s,throughput_orders_per_s,resting_orders,total_trades,total_filled_qty,latency_ns_min,latency_ns_p50,latency_ns_p95,latency_ns_p99,latency_ns_max
CSV
}

append_row() {
  local workload="$1"
  local run_type="$2"
  local run_idx="$3"
  local orders="$4"
  local seed="$5"

  local temp_csv
  temp_csv="$(mktemp)"

  "${BIN_PATH}" "${orders}" "${seed}" --csv "${temp_csv}" > /dev/null

  awk -F',' -v workload="${workload}" -v run_type="${run_type}" -v run_idx="${run_idx}" \
    '{print workload "," run_type "," run_idx "," $0}' "${temp_csv}" >> "${CSV_PATH}"

  rm -f "${temp_csv}"
}

run_matrix() {
  for entry in "${workloads[@]}"; do
    local workload
    local orders

    workload="${entry%% *}"
    orders="${entry##* }"

    echo "== ${workload} (${orders} orders) =="

    echo "  warmup"
    append_row "${workload}" "warmup" 0 "${orders}" 999

    local idx=1
    for seed in "${SEEDS[@]}"; do
      echo "  measured run ${idx} seed=${seed}"
      append_row "${workload}" "measured" "${idx}" "${orders}" "${seed}"
      idx=$((idx + 1))
    done
  done
}

build
init_csv
run_matrix

echo "Benchmark complete: ${CSV_PATH}"
