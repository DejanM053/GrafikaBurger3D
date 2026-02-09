#pragma once
#include <glm/glm.hpp>

// Light structure for Phong lighting
struct Light {
    glm::vec3 position;
    glm::vec3 color;
    float strength;
    bool enabled;
    
    // Constructor with default values
    Light() :
        position(6.0f, 7.0f, 4.0f),
        color(1.0f, 0.84f, 0.74f),
        strength(0.95f),
        enabled(true)
    {}
    
    // Constructor with custom values
    Light(glm::vec3 pos, glm::vec3 col, float str, bool en) :
        position(pos),
        color(col),
        strength(str),
        enabled(en)
    {}
};
