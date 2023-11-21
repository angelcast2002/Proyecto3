#include "cube.h"

Cube::Cube(const glm::vec3& center, float size, const Material& mat)
        : center(center), size(size), Object(mat) {
    // Inicializar los vértices del cubo
    vertices[0] = center + glm::vec3(-size / 2.0f, -size / 2.0f, -size / 2.0f);
    vertices[1] = center + glm::vec3(size / 2.0f, -size / 2.0f, -size / 2.0f);
    vertices[2] = center + glm::vec3(-size / 2.0f, -size / 2.0f, size / 2.0f);
    vertices[3] = center + glm::vec3(size / 2.0f, -size / 2.0f, size / 2.0f);
    vertices[4] = center + glm::vec3(-size / 2.0f, size / 2.0f, -size / 2.0f);
    vertices[5] = center + glm::vec3(size / 2.0f, size / 2.0f, -size / 2.0f);
    vertices[6] = center + glm::vec3(-size / 2.0f, size / 2.0f, size / 2.0f);
    vertices[7] = center + glm::vec3(size / 2.0f, size / 2.0f, size / 2.0f);
}

Intersect Cube::rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const {
    // Aplica la matriz de transformación al rayo antes de realizar la intersección
    glm::vec4 rayOriginTransformed = getTransformMatrix() * glm::vec4(rayOrigin, 1.0f);
    glm::vec4 rayDirectionTransformed = getTransformMatrix() * glm::vec4(rayDirection, 0.0f);

    glm::vec3 oc = glm::vec3(rayOriginTransformed) - center;
    float tMin = -std::numeric_limits<float>::infinity();
    float tMax = std::numeric_limits<float>::infinity();

    for (int i = 0; i < 3; ++i) {
        float invDir = 1.0f / rayDirectionTransformed[i];
        float tNear = (center[i] - size / 2.0f - glm::vec3(rayOriginTransformed)[i]) * invDir;
        float tFar = (center[i] + size / 2.0f - glm::vec3(rayOriginTransformed)[i]) * invDir;

        if (tNear > tFar) {
            std::swap(tNear, tFar);
        }

        tMin = std::max(tNear, tMin);
        tMax = std::min(tFar, tMax);

        if (tMin > tMax) {
            return Intersect{false, 0};
        }
    }

    float tHit = (tMin > 0.0f) ? tMin : tMax;

    glm::vec3 point = glm::vec3(rayOriginTransformed) + tHit * glm::vec3(rayDirectionTransformed);
    glm::vec3 normal(0.0f);

    for (int i = 0; i < 3; ++i) {
        if (point[i] < center[i] - size / 2.0f + 0.001f) {
            normal[i] = -1.0f;
        } else if (point[i] > center[i] + size / 2.0f - 0.001f) {
            normal[i] = 1.0f;
        }
    }

    return Intersect{true, tHit, point, normal};
}

