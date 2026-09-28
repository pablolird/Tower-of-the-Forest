#include "Quadtree.h"
#include <algorithm>

Quadtree::Quadtree() : Quadtree(sf::FloatRect()) {}

Quadtree::Quadtree(sf::FloatRect bounds) : root(new QuadtreeNode(bounds, 0)), bounds(bounds) {}

void Quadtree::insert(std::shared_ptr<Entity> entity) {
    auto box = entity->getComponent<CAnimation>().animation.getSprite().getGlobalBounds();
    size_t id = entity->id();
    insert(root.get(), Item{ box, id, std::move(entity) });
}

void Quadtree::clear() {
    // unique_ptr frees the whole subtree
    root.reset(new QuadtreeNode(bounds, 0));
}

std::vector<std::shared_ptr<Entity>> Quadtree::query(sf::FloatRect range) const {
    std::vector<const Item*> items;
    query(root.get(), range, items);
    // Keep the same order as a scan of the entity list, so systems that act on the
    // first match (archer targeting) behave exactly as before
    std::sort(items.begin(), items.end(), [](const Item* a, const Item* b) { return a->id < b->id; });

    std::vector<std::shared_ptr<Entity>> found;
    found.reserve(items.size());
    for (auto* item : items) found.push_back(item->entity);
    return found;
}

// Index of the child that fully contains box, or -1 if it straddles a split line
int Quadtree::childIndex(const QuadtreeNode* node, const sf::FloatRect& box) const {
    float midX = node->bounds.left + node->bounds.width / 2;
    float midY = node->bounds.top + node->bounds.height / 2;

    bool left = box.left + box.width <= midX && box.left >= node->bounds.left;
    bool right = box.left >= midX && box.left + box.width <= node->bounds.left + node->bounds.width;
    bool top = box.top + box.height <= midY && box.top >= node->bounds.top;
    bool bottom = box.top >= midY && box.top + box.height <= node->bounds.top + node->bounds.height;

    if (top && left) return 0;
    if (top && right) return 1;
    if (bottom && left) return 2;
    if (bottom && right) return 3;
    return -1;
}

void Quadtree::insert(QuadtreeNode* node, Item item) {
    if (!node->isLeaf()) {
        int i = childIndex(node, item.box);
        if (i != -1) {
            insert(node->children[i].get(), std::move(item));
            return;
        }
        node->items.push_back(std::move(item));
        return;
    }

    node->items.push_back(std::move(item));
    if (node->items.size() > MAX_ENTITIES && node->depth < MAX_DEPTH) {
        subdivide(node);
    }
}

void Quadtree::subdivide(QuadtreeNode* node) {
    float x = node->bounds.left;
    float y = node->bounds.top;
    float width = node->bounds.width / 2;
    float height = node->bounds.height / 2;
    int d = node->depth + 1;

    node->children[0].reset(new QuadtreeNode(sf::FloatRect(x, y, width, height), d));
    node->children[1].reset(new QuadtreeNode(sf::FloatRect(x + width, y, width, height), d));
    node->children[2].reset(new QuadtreeNode(sf::FloatRect(x, y + height, width, height), d));
    node->children[3].reset(new QuadtreeNode(sf::FloatRect(x + width, y + height, width, height), d));

    // Push down every item that fits entirely inside one child
    std::vector<Item> stay;
    for (auto& item : node->items) {
        int i = childIndex(node, item.box);
        if (i != -1) insert(node->children[i].get(), std::move(item));
        else stay.push_back(std::move(item));
    }
    node->items = std::move(stay);
}

static bool containsRect(const sf::FloatRect& outer, const sf::FloatRect& inner) {
    return inner.left >= outer.left && inner.top >= outer.top
        && inner.left + inner.width <= outer.left + outer.width
        && inner.top + inner.height <= outer.top + outer.height;
}

void Quadtree::query(const QuadtreeNode* node, const sf::FloatRect& range, std::vector<const Item*>& found) const {
    // The root also holds entities outside the tree bounds, so it is always scanned
    if (node != root.get()) {
        if (!node->bounds.intersects(range)) return;
        // Every entity in this subtree lies inside the node, so no per-entity test is needed.
        // Zero-size boxes never intersect anything, matching sf::FloatRect::intersects.
        if (containsRect(range, node->bounds)) {
            collectAll(node, found);
            return;
        }
    }

    for (auto& item : node->items) {
        if (range.intersects(item.box)) {
            found.push_back(&item);
        }
    }

    if (!node->isLeaf()) {
        for (int i = 0; i < 4; ++i) query(node->children[i].get(), range, found);
    }
}

void Quadtree::collectAll(const QuadtreeNode* node, std::vector<const Item*>& found) const {
    for (auto& item : node->items) {
        if (item.box.width > 0 && item.box.height > 0) found.push_back(&item);
    }
    if (!node->isLeaf()) {
        for (int i = 0; i < 4; ++i) collectAll(node->children[i].get(), found);
    }
}

size_t Quadtree::nodeCount() const { return nodeCount(root.get()); }

size_t Quadtree::nodeCount(const QuadtreeNode* node) const {
    size_t n = 1;
    if (!node->isLeaf())
        for (int i = 0; i < 4; ++i) n += nodeCount(node->children[i].get());
    return n;
}

int Quadtree::depth() const { return depth(root.get()); }

int Quadtree::depth(const QuadtreeNode* node) const {
    int d = node->depth;
    if (!node->isLeaf())
        for (int i = 0; i < 4; ++i) d = std::max(d, depth(node->children[i].get()));
    return d;
}
