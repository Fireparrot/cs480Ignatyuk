#ifndef GRAPHICS_HEADERS_H
#define GRAPHICS_HEADERS_H

#include <iostream>

#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED

#if defined(__APPLE__) || defined(MACOSX)
  #include <OpenGL/gl3.h>
  #include <OpenGL/GLU.h>
#else //linux as default
  #include <GL/glew.h>
  //#include <GL/glu.h>
#endif

// GLM for matricies
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

#define INVALID_UNIFORM_LOCATION 0x7fffffff

using usi = unsigned short int;

struct VertexData {
  glm::vec3 position;
  glm::vec2 tex;
  glm::vec3 normal;

  VertexData(glm::vec3 p, glm::vec2 t, glm::vec3 n): position(p), tex(t), normal(n) {}
};

#endif /* GRAPHICS_HEADERS_H */
