// Quadtree correctness tests: every query must return exactly what a linear scan
// over all entities returns, in the same order, with no duplicates.
#include <cstdio>
#include <random>
#include <set>
#include "../Quadtree.h"
#include "../bench/entities.h"

static int failures = 0;
#define CHECK(cond, ...) do { if (!(cond)) { std::printf("FAIL %s:%d: ", __FILE__, __LINE__); std::printf(__VA_ARGS__); std::printf("\n"); ++failures; } } while (0)

static const sf::FloatRect WORLD(0, 0, 1792, 896);

static Quadtree build(EntityVec& all) {
    Quadtree qt(WORLD);
    for (auto& e : all) qt.insert(e);
    return qt;
}

static bool sameResult(const EntityVec& a, const EntityVec& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i] != b[i]) return false;
    return true;
}

static void testRandomMatchesBruteForce() {
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> X(-100, 1892), Y(-100, 996); // some entities outside the world
    std::uniform_int_distribution<int> S(4, 150);
    std::uniform_real_distribution<float> R(10, 600);

    for (int n : { 0, 1, 9, 100, 1000, 5000 }) {
        EntityManager em;
        for (int i = 0; i < n; ++i) addBoxEntity(em, X(rng), Y(rng), S(rng), S(rng));
        em.update();
        auto& all = em.getEntities();
        auto qt = build(all);

        for (int q = 0; q < 300; ++q) {
            sf::FloatRect range(X(rng), Y(rng), R(rng), R(rng));
            auto got = qt.query(range);
            auto want = bruteForce(all, range);
            CHECK(sameResult(got, want), "n=%d query %d: got %zu hits, want %zu", n, q, got.size(), want.size());
        }
    }
}

static void testActuallySubdivides() {
    std::mt19937 rng(7);
    std::uniform_real_distribution<float> X(0, 1792), Y(0, 896);
    EntityManager em;
    for (int i = 0; i < 1000; ++i) addBoxEntity(em, X(rng), Y(rng), 32, 32);
    em.update();
    auto qt = build(em.getEntities());
    CHECK(qt.depth() > 0, "1000 spread-out entities should split the root, depth=%d", qt.depth());
    CHECK(qt.nodeCount() > 1, "expected more than one node, got %zu", qt.nodeCount());
}

static void testStackedEntitiesHitDepthLimit() {
    // 500 enemies piled on the same spot (e.g. against a barricade) must not recurse forever
    EntityManager em;
    for (int i = 0; i < 500; ++i) addBoxEntity(em, 400, 300, 2, 2);
    em.update();
    auto qt = build(em.getEntities());
    CHECK(qt.depth() <= Quadtree::MAX_DEPTH, "depth %d exceeds limit %d", qt.depth(), Quadtree::MAX_DEPTH);
    CHECK(qt.query(sf::FloatRect(390, 290, 20, 20)).size() == 500, "all stacked entities should be found");
}

static void testStraddlingEntityReturnedOnce() {
    // Centred on the world midpoint, so it crosses both split lines of the root
    EntityManager em;
    addBoxEntity(em, 896, 448, 64, 64);
    std::mt19937 rng(3);
    std::uniform_real_distribution<float> X(0, 1792), Y(0, 896);
    for (int i = 0; i < 200; ++i) addBoxEntity(em, X(rng), Y(rng), 16, 16);
    em.update();
    auto qt = build(em.getEntities());

    auto found = qt.query(WORLD);
    std::set<Entity*> unique;
    for (auto& e : found) unique.insert(e.get());
    CHECK(found.size() == 201 && unique.size() == 201, "whole-world query: %zu hits, %zu unique, want 201", found.size(), unique.size());
}

static void testOutOfBoundsEntitiesFound() {
    // Enemies spawn off-screen before walking onto the map
    EntityManager em;
    addBoxEntity(em, -50, 448, 32, 32);
    addBoxEntity(em, 1842, 448, 32, 32);
    em.update();
    auto qt = build(em.getEntities());
    CHECK(qt.query(sf::FloatRect(-100, 400, 100, 100)).size() == 1, "left off-screen entity not found");
    CHECK(qt.query(sf::FloatRect(1800, 400, 100, 100)).size() == 1, "right off-screen entity not found");
}

static void testClearEmptiesTree() {
    EntityManager em;
    for (int i = 0; i < 100; ++i) addBoxEntity(em, 10.f * i, 10.f * i, 8, 8);
    em.update();
    auto qt = build(em.getEntities());
    qt.clear();
    CHECK(qt.query(WORLD).empty(), "tree should be empty after clear()");
    CHECK(qt.nodeCount() == 1, "clear() should leave only the root, got %zu nodes", qt.nodeCount());
}

static void testEntityManagerQueryRange() {
    // Both EntityManager modes must agree
    std::mt19937 rng(11);
    std::uniform_real_distribution<float> X(0, 1792), Y(0, 896);
    EntityManager tree(WORLD), linear(WORLD);
    linear.setUseQuadtree(false);
    for (int i = 0; i < 2000; ++i) {
        float x = X(rng), y = Y(rng);
        addBoxEntity(tree, x, y, 40, 40);
        addBoxEntity(linear, x, y, 40, 40);
    }
    tree.update();
    linear.update();
    for (int q = 0; q < 200; ++q) {
        sf::FloatRect range(X(rng), Y(rng), 265 * 2, 265 * 2);
        auto a = tree.queryRange(range), b = linear.queryRange(range);
        bool same = a.size() == b.size();
        for (size_t i = 0; same && i < a.size(); ++i) same = a[i]->id() == b[i]->id();
        CHECK(same, "query %d: quadtree %zu hits vs linear %zu", q, a.size(), b.size());
    }
}

int main() {
    testRandomMatchesBruteForce();
    testActuallySubdivides();
    testStackedEntitiesHitDepthLimit();
    testStraddlingEntityReturnedOnce();
    testOutOfBoundsEntitiesFound();
    testClearEmptiesTree();
    testEntityManagerQueryRange();

    if (failures) {
        std::printf("%d check(s) failed\n", failures);
        return 1;
    }
    std::printf("All quadtree tests passed\n");
    return 0;
}
