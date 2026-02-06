#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
#include <map>

// Structure to hold vertex data for 3D models
struct Vertex {
    float x, y, z;        // Position
    float u, v;           // Texture coordinates
    float nx, ny, nz;     // Normal vectors
};

// Structure to hold a loaded 3D model
struct Model {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int vertexCount;
    
    Model() : VAO(0), VBO(0), vertexCount(0) {}
};

// Cache for loaded models to avoid loading the same model multiple times
class ModelCache {
private:
    std::map<std::string, Model> models;
    
public:
    ModelCache() {}
    ~ModelCache();
    
    // Load a model from file (or return cached version)
    // Returns the VAO handle, or 0 if loading failed
    unsigned int loadModel(const char* filepath);
    
    // Get a model by filepath (must be already loaded)
    Model* getModel(const char* filepath);
    
    // Check if a model is already loaded
    bool hasModel(const char* filepath);
    
    // Clear all loaded models
    void clear();
};
