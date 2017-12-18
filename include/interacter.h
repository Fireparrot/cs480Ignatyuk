#ifndef INTERACTER_H
#define INTERACTER_H

#include <vector>
#include <functional>
#include <utility>
#include "graphics_headers.h"

class Interactable {
    friend class Interacter;
    friend class Graphics;
private:
    glm::vec3 position;
    float radius;
    float distance;
    glm::vec3 indicatorPos;
    std::function<void (void)> function;
    bool isEnabled;
public:
    Interactable(glm::vec3 p, float r, float d, glm::vec3 ip, std::function<void ()> f, bool e);
    ~Interactable();
};

class Interacter {
private:
    std::vector<Interactable *> interactables;
public:
    Interacter();
    ~Interacter();
public:
    Interactable * add(glm::vec3 p, float r, float d, glm::vec3 ip, std::function<void ()> f, bool e = true);
    Interactable * lookingAt(glm::vec3 pos, glm::vec3 dir);
};

#endif

