#pragma once
// Helpers shared by the quadtree test and benchmark: create entities with a given
// sprite box without loading any texture (bounds come from the texture rect).
#include <random>
#include "../EntityManager.h"

inline std::shared_ptr<Entity> addBoxEntity(EntityManager& em, float x, float y, int w, int h) {
    auto e = em.addEntity("enemy");
    auto& sprite = e->addComponent<CAnimation>().animation.getSprite();
    sprite.setTextureRect(sf::IntRect(0, 0, w, h));
    sprite.setOrigin(w / 2.f, h / 2.f);
    sprite.setPosition(x, y);
    return e;
}

inline void setPosition(std::shared_ptr<Entity>& e, float x, float y) {
    e->getComponent<CAnimation>().animation.getSprite().setPosition(x, y);
}

inline sf::FloatRect boundsOf(const std::shared_ptr<Entity>& e) {
    return e->getComponent<CAnimation>().animation.getSprite().getGlobalBounds();
}

inline std::vector<std::shared_ptr<Entity>> bruteForce(EntityVec& all, const sf::FloatRect& range) {
    std::vector<std::shared_ptr<Entity>> found;
    for (auto& e : all)
        if (range.intersects(boundsOf(e))) found.push_back(e);
    return found;
}
