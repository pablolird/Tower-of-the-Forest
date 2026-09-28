#pragma once
#include <cstdlib>
#include <cstring>

// Benchmark mode: `tower-of-the-forest --stress N [--frames F] [--linear] [--seed S] [--screenshot out.png]`
// starts straight in the play scene with towers on every grass slot, barricades on every
// road slot and N invulnerable enemies spread along the roads, runs F timed frames with
// vsync off, prints one CSV line of timings and exits.
struct StressConfig {
	int enemies = 0;       // 0 = normal game
	int frames = 600;      // timed frames
	int warmup = 60;       // untimed frames before measuring
	bool quadtree = true;  // false = linear scan for range queries (the pre-fix behaviour)
	unsigned seed = 1;
	const char* screenshot = nullptr; // optional PNG of the last frame

	bool enabled() const { return enemies > 0; }

	static StressConfig fromArgs(int argc, char** argv) {
		StressConfig c;
		for (int i = 1; i < argc; ++i) {
			if (!std::strcmp(argv[i], "--stress") && i + 1 < argc) c.enemies = std::atoi(argv[++i]);
			else if (!std::strcmp(argv[i], "--frames") && i + 1 < argc) c.frames = std::atoi(argv[++i]);
			else if (!std::strcmp(argv[i], "--seed") && i + 1 < argc) c.seed = (unsigned)std::atoi(argv[++i]);
			else if (!std::strcmp(argv[i], "--screenshot") && i + 1 < argc) c.screenshot = argv[++i];
			else if (!std::strcmp(argv[i], "--linear")) c.quadtree = false;
		}
		return c;
	}
};
