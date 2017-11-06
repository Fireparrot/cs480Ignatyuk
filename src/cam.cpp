#include "cam.h"

Cam::Cam() {}

Cam::~Cam() {}

bool Cam::Initialize(int w, int h) {
    SetView(glm::vec3(0.0, 35.0, -15.0), glm::vec3(0.0, 0.0, 0.0));

    projection = glm::perspective( 45.0f,             //the FoV typically 90 degrees is good which is what this is set to
                                   float(w)/float(h), //Aspect Ratio, so Circles stay Circular
                                   0.001f,            //Distance to the near plane, normally a small value like this
                                   100.0f);           //Distance to the far plane, 
    return true;
}

glm::mat4 Cam::GetProjection() const {return projection;}
glm::mat4 Cam::GetView() const {return view;}
glm::vec3 Cam::GetPosition() const {return position;}
glm::vec3 Cam::GetTarget() const {return target;}

void Cam::SetPosition(glm::vec3 pos) {position = pos; UpdateMat();}
void Cam::SetTarget(glm::vec3 tar) {target = tar; UpdateMat();}
void Cam::SetView(glm::vec3 pos, glm::vec3 tar) {position = pos; target = tar; UpdateMat();}
void Cam::UpdateMat() {view = glm::lookAt(position, target, glm::vec3(0, 1, 0));}
