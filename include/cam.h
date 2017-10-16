#ifndef CAM_H
#define CAM_H

#include "graphics_headers.h"

class Cam {
public:
    Cam();
    ~Cam();
    bool Initialize(int w, int h);
    glm::mat4 GetProjection();
    glm::mat4 GetView();
    void SetPosition(glm::vec3 pos);
    void SetTarget(glm::vec3 tar);
    void SetView(glm::vec3 pos, glm::vec3 tar);
private:
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 position;
    glm::vec3 target;
    void UpdateMat();
};

#endif /* CAM_H */
