#version 330 core

// OpenGL 3.3 Core Profile compatible
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4 uColor; 
uniform int uUseTexture; 

// 0 = Nema zaobljenja, 1 = Dole (BunBot), 2 = Gore (BunTop)
uniform int uRounding; 

// Phong lighting uniforms
uniform vec3 uLightPos;       // Light position in world space
uniform vec3 uLightColor;     // Light color
uniform float uLightStrength; // Light intensity/strength
uniform bool uLightEnabled;   // Toggle light on/off
uniform vec3 uViewPos;        // Camera position for specular calculation

void main()
{
    // --- Logika za zaobljavanje coskova ---
    float r = 0.4; // Radijus zaobljenja (u UV koordinatama 0.0 do 1.0)
    bool discardPixel = false;
    
    // BunBot: Zaobli donje coskove
    if (uRounding == 1) {
        // Donji levi
        if (TexCoord.x < r && TexCoord.y < r && distance(TexCoord, vec2(r, r)) > r) 
            discardPixel = true;
        // Donji desni
        if (TexCoord.x > (1.0 - r) && TexCoord.y < r && distance(TexCoord, vec2(1.0 - r, r)) > r) 
            discardPixel = true;
    }
    
    // BunTop: Zaobli gornje coskove
    if (uRounding == 2) {
        // Gornji levi
        if (TexCoord.x < r && TexCoord.y > (1.0 - r) && distance(TexCoord, vec2(r, 1.0 - r)) > r) 
            discardPixel = true;
        // Gornji desni
        if (TexCoord.x > (1.0 - r) && TexCoord.y > (1.0 - r) && distance(TexCoord, vec2(1.0 - r, 1.0 - r)) > r) 
            discardPixel = true;
    }

    if (discardPixel) discard; // Izbaci piksel (providno)
    // --------------------------------------

    // Get base color from texture or uniform
    vec4 baseColor;
    if(uUseTexture == 1)
    {
        vec4 texColor = texture(uTexture, TexCoord);
        baseColor = texColor * uColor; 
    }
    else
    {
        baseColor = uColor;
    }
    
    // === PHONG LIGHTING CALCULATION ===
    vec3 finalLighting;
    
    if (uLightEnabled) {
        // Normalize the normal vector
        vec3 norm = normalize(Normal);
        
        // --- Ambient component ---
        float ambientStrength = 0.3;
        vec3 ambient = ambientStrength * uLightColor;
        
        // --- Diffuse component ---
        vec3 lightDir = normalize(uLightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * uLightColor;
        
        // --- Specular component ---
        float specularStrength = 0.5;
        vec3 viewDir = normalize(uViewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = specularStrength * spec * uLightColor;
        
        // Combine all components
        finalLighting = (ambient + diffuse + specular) * uLightStrength;
    }
    else {
        // Light disabled - use only ambient lighting (dark scene)
        float ambientStrength = 0.5;
        finalLighting = vec3(ambientStrength);
    }
    
    // Apply lighting to base color
    FragColor = vec4(baseColor.rgb * finalLighting, baseColor.a);
}