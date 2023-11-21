#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <cstdlib>
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/geometric.hpp"
#include <string>
#include "glm/glm.hpp"
#include <vector>
#include "print.h"
#include <SDL_image.h>

#include "color.h"
#include "intersect.h"
#include "object.h"
#include "sphere.h"
#include "light.h"
#include "camera.h"
#include "cube.h"


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
const int MAX_RECURSION = 3;
const float BIAS = 0.0001f;
const float FOV = 3.1415f/3.0f;

SDL_Renderer* renderer;
std::vector<Object*> objects;
Light light(glm::vec3(-1.0, 0, 10), 1.5f, Color(255, 255, 255));
Camera camera(glm::vec3(0.0, 0.0, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f);

void point(glm::vec2 position, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, position.x, position.y);
}

float castShadow(const glm::vec3& shadowOrigin, const glm::vec3& lightDir, Object* hitObject) {
    for (auto& obj : objects) {
        if (obj != hitObject) {
            Intersect shadowIntersect = obj->rayIntersect(shadowOrigin, lightDir);
            if (shadowIntersect.isIntersecting && shadowIntersect.dist > 0) {
                float shadowRatio = shadowIntersect.dist / glm::length(light.position - shadowOrigin);
                shadowRatio = glm::min(1.0f, shadowRatio);
                return 1.0f - shadowRatio;
            }
        }
    }
    return 1.0f;
}

Color castRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const short recursion = 0) {
    float zBuffer = 99999;
    Object* hitObject = nullptr;
    Intersect intersect;

    for (const auto& object : objects) {
        Intersect i = object->rayIntersect(rayOrigin, rayDirection);
        if (i.isIntersecting && i.dist < zBuffer) {
            zBuffer = i.dist;
            hitObject = object;
            intersect = i;
        }
    }

    if (!intersect.isIntersecting || recursion == MAX_RECURSION) {
        return Color(173, 216, 230);
    }

    // Transforma la dirección de la luz y la dirección de la vista al espacio del objeto
    glm::vec3 lightDirObjSpace = glm::mat3(glm::transpose(glm::inverse(hitObject->getTransformMatrix()))) * glm::normalize(light.position - intersect.point);
    glm::vec3 viewDirObjSpace = glm::mat3(glm::transpose(glm::inverse(hitObject->getTransformMatrix()))) * glm::normalize(rayOrigin - intersect.point);

    glm::vec3 reflectDirObjSpace = glm::reflect(-lightDirObjSpace, intersect.normal);

    float shadowIntensity = castShadow(intersect.point, lightDirObjSpace, hitObject);

    float diffuseLightIntensity = glm::max(0.0f, glm::dot(intersect.normal, lightDirObjSpace));
    float specLightIntensity = std::pow(glm::max(0.0f, glm::dot(viewDirObjSpace, reflectDirObjSpace)), hitObject->material.specularCoefficient);

    // Reflección y refracción
    Color reflectedColor(0.0f, 0.0f, 0.0f);
    if (hitObject->material.reflectivity > 0) {
        glm::vec3 origin = intersect.point + intersect.normal * BIAS;
        glm::vec3 reflectedRayDirObjSpace = glm::mat3(glm::transpose(glm::inverse(hitObject->getTransformMatrix()))) * reflectDirObjSpace;
        reflectedColor = castRay(origin, reflectedRayDirObjSpace, recursion + 1);
    }

    Color refractedColor(0.0f, 0.0f, 0.0f);
    if (hitObject->material.transparency > 0) {
        glm::vec3 origin = intersect.point - intersect.normal * BIAS;
        glm::vec3 refractDirObjSpace = glm::mat3(glm::transpose(glm::inverse(hitObject->getTransformMatrix()))) * glm::refract(rayDirection, intersect.normal, hitObject->material.refractionIndex);
        refractedColor = castRay(origin, refractDirObjSpace, recursion + 1);
    }

    // Cálculos de luz difusa y especular
    Color diffuseLight = hitObject->material.diffuse * light.intensity * diffuseLightIntensity * hitObject->material.albedo * shadowIntensity;
    Color specularLight = light.color * light.intensity * specLightIntensity * hitObject->material.specularAlbedo * shadowIntensity;

    // Combinación de los componentes de iluminación y efectos
    Color color = (diffuseLight + specularLight) * (1.0f - hitObject->material.reflectivity - hitObject->material.transparency) + reflectedColor * hitObject->material.reflectivity + refractedColor * hitObject->material.transparency;
    return color;
}

void setUp() {
    Material rubber = {
        Color(80, 0, 0),   // diffuse
        0.9,
        0.1,
        10.0f,
        0.0f,
        0.0f
    };

    Material ivory = {
        Color(100, 100, 80),
        0.5,
        0.5,
        50.0f,
        0.4f,
        0.0f
    };

    Material mirror = {
        Color(255, 255, 255),
        0.0f,
        10.0f,
        1425.0f,
        0.9f,
        0.0f
    };

    Material glass = {
        Color(255, 255, 255),
        0.0f,
        10.0f,
        1425.0f,
        0.2f,
        1.0f,
    };
    /*
    objects.push_back(new Sphere(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, rubber));
    objects.push_back(new Sphere(glm::vec3(-1.0f, 0.0f, -4.0f), 1.0f, ivory));
    objects.push_back(new Sphere(glm::vec3(1.0f, 0.0f, -4.0f), 1.0f, mirror));
    */


    objects.push_back(new Cube(glm::vec3(0.0f, 0.0f, -0.0f), 1.0f, rubber));
    /*objects.push_back(new Cube(glm::vec3(0.0f, 1.0f, -3.0f), 1.0f, glass));
    objects.push_back(new Cube(glm::vec3(-1.0f, 0.0f, -4.0f), 1.0f, ivory));
    objects.push_back(new Cube(glm::vec3(1.0f, 0.0f, -4.0f), 1.0f, mirror));*/
}

void render() {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            float screenX = (2.0f * (x + 0.5f)) / SCREEN_WIDTH - 1.0f;
            float screenY = -(2.0f * (y + 0.5f)) / SCREEN_HEIGHT + 1.0f;
            screenX *= ASPECT_RATIO;
            screenX *= tan(FOV / 2.0f);
            screenY *= tan(FOV / 2.0f);

            glm::vec3 cameraDir = glm::normalize(camera.target - camera.position);
            glm::vec3 cameraX = glm::normalize(glm::cross(cameraDir, camera.up));
            glm::vec3 cameraY = glm::normalize(glm::cross(cameraX, cameraDir));
            glm::vec3 rayDirection = glm::normalize(
                    cameraDir + cameraX * screenX + cameraY * screenY
            );

            Color pixelColor = castRay(camera.position, rayDirection);

            // Obtén el objeto más cercano al rayo
            Object* hitObject = nullptr;
            Intersect intersect;
            float zBuffer = 99999;

            for (const auto& object : objects) {
                Intersect i = object->rayIntersect(camera.position, rayDirection);
                if (i.isIntersecting && i.dist < zBuffer) {
                    zBuffer = i.dist;
                    hitObject = object;
                    intersect = i;
                }
            }

            if (hitObject) {
                SDL_Texture* texture = hitObject->getTexture();
                if (texture) {
                    // Calcula la posición del píxel en las coordenadas de la textura
                    float u = (float)x / SCREEN_WIDTH;
                    float v = (float)y / SCREEN_HEIGHT;

                    // Calcula las coordenadas de textura usando las coordenadas UV
                    int texX = static_cast<int>(u * 128);
                    int texY = static_cast<int>(v * 128);

                    // Crea un rectángulo de destino para renderizar la textura en el píxel
                    SDL_Rect destRect = { x, y, 1, 1 };

                    // Renderiza la textura en el píxel
                    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
                } else {
                    // Renderiza el color sólido si no hay textura
                    point(glm::vec2(x, y), pixelColor);
                }
            } else {
                // Renderiza el color sólido si no hay intersección
                point(glm::vec2(x, y), pixelColor);
            }
        }
    }
}


int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    int imgFlags = IMG_INIT_PNG;  // O ajusta según el formato de tu imagen
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        // Manejo de error si la inicialización falla
        SDL_Log("Failed to initialize SDL_image: %s", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface* surface = IMG_Load("C:\\Users\\caste\\OneDrive\\Documentos\\Universidad\\semestre6\\graficosxcomputador\\Proyecto3\\assets\\STONE2.png");
    if (!surface) {
        // Manejo de error si la carga de la imagen falla
        SDL_Log("Failed to load texture: %s", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);  // Libera la superficie, ya que ya no la necesitamos


    // Create a window
    SDL_Window* window = SDL_CreateWindow("Hello World - FPS: 0", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          SCREEN_WIDTH, SCREEN_HEIGHT, 
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    int frameCount = 0;
    Uint32 startTime = SDL_GetTicks();
    Uint32 currentTime = startTime;
    
    setUp();

    if (!objects.empty()) {
        objects[0]->setTexture(texture);
    } else {
        SDL_Log("No objects created!");
        SDL_DestroyTexture(texture);
        SDL_Quit();
        return 1;
    }

    float rotationSpeed = 0.5f;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        camera.move(1.0f);
                        break;
                    case SDLK_DOWN:
                        camera.move(-1.0f);
                        break;
                    case SDLK_LEFT:
                        print("left");
                        camera.rotate(-1.0f, 0.0f);
                        break;
                    case SDLK_RIGHT:
                        print("right");
                        camera.rotate(1.0f, 0.0f);
                        break;
                 }
            }


        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        objects[0]->rotate(rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));

        render();

        // Present the renderer
        SDL_RenderPresent(renderer);

        frameCount++;

        // Calculate and display FPS
        if (SDL_GetTicks() - currentTime >= 1000) {
            currentTime = SDL_GetTicks();
            std::string title = "Hello World - FPS: " + std::to_string(frameCount);
            SDL_SetWindowTitle(window, title.c_str());
            frameCount = 0;
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

