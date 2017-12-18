#include "interacter.h"

Interactable::Interactable(glm::vec3 p, float r, float d, glm::vec3 ip, std::function<void ()> f, bool e):
    position(p),
    radius(r),
    distance(d),
    indicatorPos(ip),
    function(f),
    isEnabled(e)
{}
Interactable::~Interactable() {}

Interacter::Interacter() {}
Interacter::~Interacter() {for(Interactable * i : interactables) {delete i;}}

Interactable * Interacter::add(glm::vec3 p, float r, float d, glm::vec3 ip, std::function<void ()> f, bool e) {
    Interactable * i = new Interactable(p, r, d, ip, f, e);
    interactables.push_back(i);
    return i;
}
Interactable * Interacter::lookingAt(glm::vec3 pos, glm::vec3 dir) {
    dir = glm::normalize(dir);      //Just in case
    Interactable * answer = NULL;
    float minD = 0;
    for(Interactable * i : interactables) {
        glm::vec3 to = i->position-pos;
        float d = glm::dot(to, dir);
        float r = glm::length(to);
        r = sqrt(r*r-d*d);
        if(r <= i->radius && d <= i->distance) {
            if(d < minD || minD == 0) {answer = i; minD = d;}
        }
    }
    return answer;
}

