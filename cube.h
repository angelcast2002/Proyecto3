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
    const int cubeFaces[6][3] = {
            {0, 1, 2},
            {1, 3, 2},
            {4, 6, 5},
            {5, 6, 7},
            {0, 2, 4},
            {2, 6, 4}
    };
    glm::vec3 vertices[8];
    glm::vec3 center;  // Se agregó el miembro 'center'
    float size;       // Se agregó el miembro 'size'
};
