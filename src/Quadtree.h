#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Entity.h"

// Region quadtree over entity sprite bounds, rebuilt once per frame by EntityManager.
// Each entity is stored exactly once, in the deepest node that fully contains its
// bounds; entities that straddle a split line stay in the parent. Entities outside
// the tree bounds are kept in the root, so queries never miss or duplicate an entity.
class Quadtree {
public:
    static constexpr size_t MAX_ENTITIES = 8; // entities a leaf holds before it splits
    static constexpr int MAX_DEPTH = 6;       // stops endless splitting when entities stack up

    Quadtree();
    Quadtree(sf::FloatRect bounds);
    void insert(std::shared_ptr<Entity> entity);
    void clear();
    // Entities whose bounds intersect range, in creation (id) order like the entity list
    std::vector<std::shared_ptr<Entity>> query(sf::FloatRect range) const;

    size_t nodeCount() const;
    int depth() const;

private:
    struct Item {
        sf::FloatRect box; // bounds when inserted, so the tree stays consistent within a frame
        size_t id;         // cached so sorting results doesn't dereference every entity
        std::shared_ptr<Entity> entity;
    };

    struct QuadtreeNode {
        sf::FloatRect bounds;
        int depth;
        std::vector<Item> items;
        std::unique_ptr<QuadtreeNode> children[4];

        QuadtreeNode(sf::FloatRect b, int d) : bounds(b), depth(d) {}
        bool isLeaf() const { return !children[0]; }
    };

    std::unique_ptr<QuadtreeNode> root;
    sf::FloatRect bounds;

    void insert(QuadtreeNode* node, Item item);
    void subdivide(QuadtreeNode* node);
    int childIndex(const QuadtreeNode* node, const sf::FloatRect& box) const;
    void query(const QuadtreeNode* node, const sf::FloatRect& range, std::vector<const Item*>& found) const;
    void collectAll(const QuadtreeNode* node, std::vector<const Item*>& found) const;
    size_t nodeCount(const QuadtreeNode* node) const;
    int depth(const QuadtreeNode* node) const;
};
