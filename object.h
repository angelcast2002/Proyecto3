#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "material.h"
#include "intersect.h"
#include <SDL.h>

class Object {
public:
    Object(const Material& mat) : material(mat), position(glm::vec3(0.0f)), rotationAxis(glm::vec3(0.0f, 1.0f, 0.0f)), rotationAngle(0.0f), scale(glm::vec3(1.0f)), texture(nullptr) {}

    virtual Intersect rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const = 0;

    // Funciones para transformaciones
    void translate(const glm::vec3& translation) { position += translation; }
    void rotate(float angle, const glm::vec3& axis) {
        rotationAngle += angle;
        rotationAxis = glm::normalize(rotationAxis + axis);
    }
    void scaleObject(const glm::vec3& scaleFactor) { scale *= scaleFactor; }

    // Función para obtener la matriz de transformación del objeto
    glm::mat4 getTransformMatrix() const {
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
        return translationMatrix * rotationMatrix * scaleMatrix;
    }

    void setTexture(SDL_Texture* tex) {
        texture = tex;
    }

    SDL_Texture* getTexture() const {
        return texture;
    }

    glm::vec3 position;
    glm::vec3 rotationAxis;
    float rotationAngle;
    glm::vec3 scale;
    Material material;

private:
    SDL_Texture* texture;
};
