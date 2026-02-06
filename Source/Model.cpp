#include "../Header/Model.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

ModelCache::~ModelCache() {
    clear();
}

void ModelCache::clear() {
    for (auto& pair : models) {
        Model& model = pair.second;
        if (model.VBO != 0) {
            glDeleteBuffers(1, &model.VBO);
        }
        if (model.VAO != 0) {
            glDeleteVertexArrays(1, &model.VAO);
        }
    }
    models.clear();
}

bool ModelCache::hasModel(const char* filepath) {
    return models.find(filepath) != models.end();
}

Model* ModelCache::getModel(const char* filepath) {
    auto it = models.find(filepath);
    if (it != models.end()) {
        return &(it->second);
    }
    return nullptr;
}

// Parse OBJ file and create OpenGL buffers
unsigned int ModelCache::loadModel(const char* filepath) {
    // Check if already loaded
    if (hasModel(filepath)) {
        Model* existing = getModel(filepath);
        if (existing) {
            return existing->VAO;
        }
    }
    
    std::cout << "Loading OBJ model: " << filepath << std::endl;
    
    // Temporary storage for OBJ data
    std::vector<float> temp_positions;
    std::vector<float> temp_texcoords;
    std::vector<float> temp_normals;
    
    // Final interleaved vertex data
    std::vector<Vertex> vertices;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "ERROR: Could not open OBJ file: " << filepath << std::endl;
        return 0;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "v") {
            // Vertex position
            float x, y, z;
            iss >> x >> y >> z;
            temp_positions.push_back(x);
            temp_positions.push_back(y);
            temp_positions.push_back(z);
        }
        else if (prefix == "vt") {
            // Texture coordinate
            float u, v;
            iss >> u >> v;
            temp_texcoords.push_back(u);
            temp_texcoords.push_back(v);
        }
        else if (prefix == "vn") {
            // Normal
            float nx, ny, nz;
            iss >> nx >> ny >> nz;
            temp_normals.push_back(nx);
            temp_normals.push_back(ny);
            temp_normals.push_back(nz);
        }
        else if (prefix == "f") {
            // Face (assuming triangles)
            // Format: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
            std::string vertex1, vertex2, vertex3;
            iss >> vertex1 >> vertex2 >> vertex3;
            
            std::string vertexData[3] = {vertex1, vertex2, vertex3};
            
            for (int i = 0; i < 3; i++) {
                Vertex vert;
                int posIdx = 0, texIdx = 0, normIdx = 0;
                
                // Parse vertex data (format: pos/tex/norm or pos//norm or pos/tex or pos)
                size_t slash1 = vertexData[i].find('/');
                if (slash1 != std::string::npos) {
                    posIdx = std::stoi(vertexData[i].substr(0, slash1));
                    
                    size_t slash2 = vertexData[i].find('/', slash1 + 1);
                    if (slash2 != std::string::npos) {
                        if (slash2 > slash1 + 1) {
                            texIdx = std::stoi(vertexData[i].substr(slash1 + 1, slash2 - slash1 - 1));
                        }
                        normIdx = std::stoi(vertexData[i].substr(slash2 + 1));
                    }
                    else {
                        // Only position and texture
                        texIdx = std::stoi(vertexData[i].substr(slash1 + 1));
                    }
                }
                else {
                    // Only position
                    posIdx = std::stoi(vertexData[i]);
                }
                
                // OBJ indices are 1-based, convert to 0-based
                posIdx--; texIdx--; normIdx--;
                
                // Set position
                if (posIdx >= 0 && posIdx * 3 + 2 < (int)temp_positions.size()) {
                    vert.x = temp_positions[posIdx * 3 + 0];
                    vert.y = temp_positions[posIdx * 3 + 1];
                    vert.z = temp_positions[posIdx * 3 + 2];
                } else {
                    vert.x = vert.y = vert.z = 0.0f;
                }
                
                // Set texture coordinates
                if (texIdx >= 0 && texIdx * 2 + 1 < (int)temp_texcoords.size()) {
                    vert.u = temp_texcoords[texIdx * 2 + 0];
                    vert.v = temp_texcoords[texIdx * 2 + 1];
                } else {
                    vert.u = vert.v = 0.0f;
                }
                
                // Set normals
                if (normIdx >= 0 && normIdx * 3 + 2 < (int)temp_normals.size()) {
                    vert.nx = temp_normals[normIdx * 3 + 0];
                    vert.ny = temp_normals[normIdx * 3 + 1];
                    vert.nz = temp_normals[normIdx * 3 + 2];
                } else {
                    vert.nx = 0.0f; vert.ny = 1.0f; vert.nz = 0.0f; // Default up
                }
                
                vertices.push_back(vert);
            }
        }
    }
    
    file.close();
    
    if (vertices.empty()) {
        std::cout << "ERROR: No vertices loaded from OBJ file: " << filepath << std::endl;
        return 0;
    }
    
    std::cout << "Loaded " << vertices.size() << " vertices from " << filepath << std::endl;
    
    // Create OpenGL buffers
    Model model;
    model.vertexCount = vertices.size();
    
    glGenVertexArrays(1, &model.VAO);
    glGenBuffers(1, &model.VBO);
    
    glBindVertexArray(model.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, model.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    
    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute (location 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Normal attribute (location 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    // Store in cache
    models[filepath] = model;
    
    return model.VAO;
}
