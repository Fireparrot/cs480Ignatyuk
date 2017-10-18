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
    
    //Sets the position of the camera, and updates its view matrix accordingly (keeping the camera's points-to position the same)
    void SetPosition(glm::vec3 pos);
    
    //Sets the camera's points-to position, and updates its view matrix accordingly (keeping the camera's position the same)
    void SetTarget(glm::vec3 tar);
    
    //Sets both the camera's position, and its points-to position, and updates its view matrix accordingly
    void SetView(glm::vec3 pos, glm::vec3 tar);
private:
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 position;
    glm::vec3 target;
    
    //Updates the view matrix using the stored position and points-to position
    void UpdateMat();
};

#endif /* CAM_H */
