#pragma once
// The original Quadtree (commit d0c273f, Jun 2024; unchanged until the fix), kept only so the benchmark can
// compare against it. Its insert() never subdivides: children[0] is always null, so
// every entity lands in the root and each query is a linear scan plus overhead.
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../Entity.h"

class LegacyQuadtree {
public:
    LegacyQuadtree(sf::FloatRect bounds) : root(new QuadtreeNode(bounds)), bounds(bounds) {}
    void insert(std::shared_ptr<Entity> entity) { insert(root, entity); }
    void clear() { delete root; root = new QuadtreeNode(bounds); }
    std::vector<std::shared_ptr<Entity>> query(sf::FloatRect range) {
        std::vector<std::shared_ptr<Entity>> found;
        query(root, range, found);
        return found;
    }

private:
    static const int MAX_ENTITIES = 4;
    struct QuadtreeNode {
        sf::FloatRect bounds;
        std::vector<std::shared_ptr<Entity>> entities;
        QuadtreeNode* children[4] = { nullptr, nullptr, nullptr, nullptr };
        QuadtreeNode(sf::FloatRect b) : bounds(b) {}
    };

    QuadtreeNode* root;
    sf::FloatRect bounds;

    void insert(QuadtreeNode* node, std::shared_ptr<Entity> entity) {
        if (!node->bounds.intersects(entity->getComponent<CAnimation>().animation.getSprite().getGlobalBounds())) return;

        if (node->entities.size() < MAX_ENTITIES || node->children[0] == nullptr) {
            node->entities.push_back(entity);
        }
        else {
            if (node->children[0] == nullptr) subdivide(node);
            for (int i = 0; i < 4; ++i) insert(node->children[i], entity);
        }
    }

    void subdivide(QuadtreeNode* node) {
        float x = node->bounds.left;
        float y = node->bounds.top;
        float width = node->bounds.width / 2;
        float height = node->bounds.height / 2;

        node->children[0] = new QuadtreeNode(sf::FloatRect(x, y, width, height));
        node->children[1] = new QuadtreeNode(sf::FloatRect(x + width, y, width, height));
        node->children[2] = new QuadtreeNode(sf::FloatRect(x, y + height, width, height));
        node->children[3] = new QuadtreeNode(sf::FloatRect(x + width, y + height, width, height));
    }

    void query(QuadtreeNode* node, sf::FloatRect range, std::vector<std::shared_ptr<Entity>>& found) {
        if (!node->bounds.intersects(range)) return;

        for (auto& entity : node->entities) {
            if (range.intersects(entity->getComponent<CAnimation>().animation.getSprite().getGlobalBounds())) {
                found.push_back(entity);
            }
        }

        if (node->children[0] != nullptr) {
            for (int i = 0; i < 4; ++i) query(node->children[i], range, found);
        }
    }
};
