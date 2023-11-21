#pragma once

#include "glm/glm.hpp"
#include "object.h"
#include "material.h"
#include "intersect.h"

class Cube : public Object {
public:
    Cube(const glm::vec3& center, float size, const Material& mat);

    Intersect rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const override;

private:
    glm::vec3 center;  // Se agregó el miembro 'center'
    float size;       // Se agregó el miembro 'size'
};
