#include "cube.h"

Cube::Cube(const glm::vec3& center, float size, const Material& mat)
        : center(center), size(size), Object(mat) {
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

    // Calcula las coordenadas de textura para cualquier cubo en el espacio
    float tx, ty;

    // Proyecta las coordenadas del punto de intersección en cada cara del cubo sobre un plano 2D
    glm::vec3 hitPoint = glm::vec3(rayOriginTransformed) + tHit * glm::vec3(rayDirectionTransformed);
    glm::vec3 localHitPoint = hitPoint - center;

    if (std::abs(normal.x) > 0) {
        tx = (localHitPoint.z / size) + 0.5f;
        ty = (localHitPoint.y / size) + 0.5f;
    } else if (std::abs(normal.y) > 0) {
        tx = (localHitPoint.x / size) + 0.5f;
        ty = (localHitPoint.z / size) + 0.5f;
    } else {
        tx = (localHitPoint.x / size) + 0.5f;
        ty = (localHitPoint.y / size) + 0.5f;
    }

    // Asegúrate de que las coordenadas UV estén en el rango [0, 1]
    tx = tx - floor(tx);
    ty = ty - floor(ty);

    return Intersect{true, tHit, point, normal, tx, ty};
}
