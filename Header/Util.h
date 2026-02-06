#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GameObject.h"
#include "Camera.h"

// Forward declarations
class ModelCache;

int endProgram(std::string message);
unsigned int createShader(const char* vsSource, const char* fsSource);
unsigned loadImageToTexture(const char* filePath);
GLFWcursor* loadImageToCursor(const char* filePath);

// 3D model loading
unsigned int loadOBJModel(const char* filepath, ModelCache& cache);

// Render a 3D model or 2D quad based on GameObject settings
void RenderObject3D(unsigned int shader, unsigned int quadVAO, GameObject& obj, 
                    Camera& camera, float aspectRatio, ModelCache& cache, int roundingMode = 0);