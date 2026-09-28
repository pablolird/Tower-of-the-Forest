// Headless spatial-query benchmark: original quadtree vs linear scan vs fixed quadtree.
//
// Each simulated frame moves every entity, rebuilds the index, then runs a batch of
// range queries. Two workloads:
//   game    - the queries the game actually makes per frame: 16 archer ranges
//             (530x530 box), 15 barricades + the main tower (200x200 box)
//   all     - every entity queries a 64x64 box around itself (all-pairs collision)
// Entities are 48x48 boxes spread uniformly over the 1792x896 map.
#include <chrono>
#include <cstdio>
#include <cstring>
#include <functional>
#include <random>
#include "../Quadtree.h"
#include "legacy_quadtree.h"
#include "entities.h"

using Clock = std::chrono::steady_clock;
static const sf::FloatRect WORLD(0, 0, 1792, 896);

struct Workload {
    const char* name;
    std::function<std::vector<sf::FloatRect>(EntityVec&)> queries;
};

static std::vector<sf::FloatRect> gameQueries(EntityVec&) {
    std::vector<sf::FloatRect> q;
    // 16 grass slots for archers (range 265) and 15 road slots for barricades, as in Scene_Play
    for (int i = 0; i < 16; ++i) {
        float x = 64.f + 110.f * i, y = (i % 2) ? 320.f : 190.f;
        q.emplace_back(x - 265, y - 265, 530, 530);
    }
    for (int i = 0; i < 15; ++i) {
        float x = 128.f + 112.f * i, y = 512.f;
        q.emplace_back(x - 100, y - 100, 200, 200);
    }
    q.emplace_back(896 - 100, 498 - 100, 200, 200);
    return q;
}

static std::vector<sf::FloatRect> allPairsQueries(EntityVec& all) {
    std::vector<sf::FloatRect> q;
    q.reserve(all.size());
    for (auto& e : all) {
        auto b = boundsOf(e);
        q.emplace_back(b.left - 8, b.top - 8, b.width + 16, b.height + 16);
    }
    return q;
}

// Runs `frames` frames and returns average milliseconds per frame (rebuild + queries)
template <typename Build, typename Query>
static double runFrames(EntityVec& all, const Workload& w, int frames, Build build, Query query, size_t& checksum) {
    std::mt19937 rng(1);
    std::uniform_real_distribution<float> step(-2, 2);
    double totalMs = 0;
    for (int f = 0; f < frames; ++f) {
        for (auto& e : all) {
            auto& s = e->getComponent<CAnimation>().animation.getSprite();
            auto p = s.getPosition();
            s.setPosition(std::min(std::max(p.x + step(rng), 0.f), 1792.f), std::min(std::max(p.y + step(rng), 0.f), 896.f));
        }
        auto queries = w.queries(all);

        auto t0 = Clock::now();
        build(all);
        for (auto& r : queries) checksum += query(r);
        totalMs += std::chrono::duration<double, std::milli>(Clock::now() - t0).count();
    }
    return totalMs / frames;
}

int main(int argc, char** argv) {
    int frames = argc > 1 ? std::atoi(argv[1]) : 60;
    const int sizes[] = { 100, 250, 500, 1000, 2000, 5000, 10000 };
    Workload workloads[] = { { "game", gameQueries }, { "all", allPairsQueries } };

    std::printf("workload,entities,queries_per_frame,original_ms,linear_ms,quadtree_ms,speedup_vs_original\n");
    for (auto& w : workloads) {
        for (int n : sizes) {
            if (std::strcmp(w.name, "all") == 0 && n > 5000) continue; // original takes minutes here

            // Identical starting positions for every implementation
            auto makeEntities = [&](EntityManager& em) {
                std::mt19937 rng(n);
                std::uniform_real_distribution<float> X(0, 1792), Y(0, 896);
                for (int i = 0; i < n; ++i) addBoxEntity(em, X(rng), Y(rng), 48, 48);
                em.update();
            };
            size_t sumOriginal = 0, sumLinear = 0, sumTree = 0;

            EntityManager emA; makeEntities(emA);
            LegacyQuadtree legacy(WORLD);
            double original = runFrames(emA.getEntities(), w, frames,
                [&](EntityVec& all) { legacy.clear(); for (auto& e : all) legacy.insert(e); },
                [&](const sf::FloatRect& r) { return legacy.query(r).size(); }, sumOriginal);

            EntityManager emB; makeEntities(emB);
            double linear = runFrames(emB.getEntities(), w, frames,
                [](EntityVec&) {},
                [&](const sf::FloatRect& r) { return bruteForce(emB.getEntities(), r).size(); }, sumLinear);

            EntityManager emC; makeEntities(emC);
            Quadtree tree(WORLD);
            double fixed = runFrames(emC.getEntities(), w, frames,
                [&](EntityVec& all) { tree.clear(); for (auto& e : all) tree.insert(e); },
                [&](const sf::FloatRect& r) { return tree.query(r).size(); }, sumTree);

            if (sumOriginal != sumLinear || sumLinear != sumTree) {
                std::fprintf(stderr, "result mismatch at %s n=%d: %zu %zu %zu\n", w.name, n, sumOriginal, sumLinear, sumTree);
                return 1;
            }
            size_t q = w.queries(emC.getEntities()).size();
            std::printf("%s,%d,%zu,%.3f,%.3f,%.3f,%.1f\n", w.name, n, q, original, linear, fixed, original / fixed);
            std::fflush(stdout);
        }
    }
    return 0;
}
