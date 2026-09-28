#!/usr/bin/env bash
# In-game stress test: runs the real game loop (all systems + rendering, vsync off) with a
# fixed number of invulnerable enemies, once with the quadtree and once with a linear scan.
# Prints the median of RUNS runs per configuration as CSV.
#   Usage: bench/stress.sh [counts...]     (env: FRAMES=600 RUNS=3)
set -euo pipefail
cd "$(dirname "$0")/.."
BIN=${BIN:-./build/tower-of-the-forest}
FRAMES=${FRAMES:-600}
RUNS=${RUNS:-3}
COUNTS=${*:-250 500 1000 2000 4000 8000 16000}

median() { sort -n | awk '{a[NR]=$1} END{print a[int((NR+1)/2)]}'; }

echo "index,enemies,entities,frames,sim_ms,render_ms,frame_ms,fps,checksum"
for n in $COUNTS; do
  for mode in ${MODES:-quadtree linear}; do
    flag=""; [ "$mode" = linear ] && flag="--linear"
    rows=$(for _ in $(seq "$RUNS"); do
      "$BIN" --stress "$n" --frames "$FRAMES" $flag | grep -E '^(quadtree|linear),'
    done)
    # Median run by total frame time; the checksum must match across modes for the same n
    echo "$rows" | sort -t, -k7 -n | sed -n "$(( (RUNS + 1) / 2 ))p"
  done
done
