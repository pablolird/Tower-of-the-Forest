# Benchmark results

Measured 28 Sep 2026 on an Apple M1 Pro, macOS 26.6, Apple clang 21 (`-O2`), SFML 2.6.2.

| File | Produced by | What it compares |
|---|---|---|
| `stress_original.csv` | `bench/stress.sh` built at commit `19e3699` (original quadtree and targeting) | In-game frame timings before the fix |
| `stress_fixed.csv` | `bench/stress.sh` at commit `837727d` | After the fix: `quadtree` rows vs a `linear` scan with the same targeting code |
| `bench_quadtree.csv` | `make bench` (`bench_quadtree 10`) | Headless spatial-query cost: original tree vs linear scan vs fixed tree |

Stress runs: 600 timed frames after 60 warm-up frames, vsync off, median of 3 runs. The game window
was 1792×896 requested but shrunk by macOS to fit a smaller display, so absolute FPS is specific to
this machine; compare rows against each other. Identical `checksum` values for the same enemy count
mean every build simulated exactly the same game.
