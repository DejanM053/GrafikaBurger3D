#pragma once
#include <string>

struct GameObject {
    float x, y, z;           // 3D position (z added)
    float w, h, d;           // 3D dimensions (d=depth added)
    float r, g, b, a;        // Color and alpha
    float rotateX, rotateY, rotateZ; // Rotation angles in degrees
    unsigned int textureId;
    bool useTexture;
    bool isVisible;
    
    // 3D model support
    bool is3DModel;          // If true, render using modelVAO instead of quad
    unsigned int modelVAO;   // VAO handle for 3D model (0 means use quad)
    std::string modelPath;   // Path to the model file for cache lookup
    
    GameObject() : 
        x(0), y(0), z(0), 
        w(0.1f), h(0.1f), d(0.1f), 
        r(1), g(1), b(1), a(1), 
        rotateX(0), rotateY(0), rotateZ(0),
        textureId(0), useTexture(false), isVisible(true),
        is3DModel(false), modelVAO(0), modelPath("") {}
};
