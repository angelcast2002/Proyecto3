cmake_minimum_required(VERSION 3.26)
project(Proyecto3)

set(CMAKE_CXX_STANDARD 20)

set(SDL2_INCLUDE_DIR C:/Users/caste/OneDrive/Documentos/SDL2-2.28.1/include)
set(SDL2_LIB_DIR C:/Users/caste/OneDrive/Documentos/SDL2-2.28.1/lib/x64)

include_directories(${FMOD_INCLUDE_DIR})
link_directories(${FMOD_LIBRARY})

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

add_executable(Proyecto3 main.cpp
        camera.cpp
        sphere.cpp
        cube.h
        cube.cpp)

target_link_libraries(${PROJECT_NAME}
        ${FMOD_LIBRARY}
        SDL2main SDL2
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
        )
