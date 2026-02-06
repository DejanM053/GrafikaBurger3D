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

    vec4 finalColor;
    if(uUseTexture == 1)
    {
        vec4 texColor = texture(uTexture, TexCoord);
        finalColor = texColor * uColor; 
    }
    else
    {
        finalColor = uColor;
    }
    
    // Simple lighting calculation (optional - can be disabled by setting light to 1.0)
    // Assumes a light from above-front
    vec3 lightDir = normalize(vec3(0.0, 1.0, 1.0));
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    float ambientStrength = 0.6;
    float light = ambientStrength + (1.0 - ambientStrength) * diff;
    
    FragColor = vec4(finalColor.rgb * light, finalColor.a);
}