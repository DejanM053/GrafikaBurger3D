#version 330 core

// U Core profilu je najbolja praksa eksplicitno navesti lokacije
// koje se poklapaju sa glVertexAttribPointer(0, ...) i (1, ...) i (2, ...) u C++ kodu.
layout (location = 0) in vec3 aPos; // Changed to vec3 for 3D
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal; // Normal vector for 3D models

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

// Legacy 2D uniforms - kept for backward compatibility during transition
uniform vec2 uPos; 
uniform vec2 uScale; 

void main()
{
    // 3D transformation pipeline
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    gl_Position = uProjection * uView * worldPos;
    
    TexCoord = aTexCoord;
    Normal = mat3(transpose(inverse(uModel))) * aNormal; // Transform normal to world space
}