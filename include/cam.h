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
  
private:
    glm::mat4 projection;
    glm::mat4 view;
};

#endif /* CAM_H */
