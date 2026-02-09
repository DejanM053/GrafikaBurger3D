#include "../Header/Util.h"
#include "../Header/Model.h"

#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <sstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../Header/stb_image.h"

// Autor: Nedeljko Tesanovic
// Opis: pomocne funkcije za zaustavljanje programa, ucitavanje sejdera, tekstura i kursora
// Smeju se koristiti tokom izrade projekta

int endProgram(std::string message) {
    std::cout << message << std::endl;
    glfwTerminate();
    return -1;
}

unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)

    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

unsigned int loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        // Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);

        // --- OVO SU LINIJE KOJE SU NEDOSTAJALE ---
        // Podesavanje parametara teksture (kako se slika ponasa kad se smanjuje/poveca)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Ili GL_LINEAR ako ne koristis mipmape
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // ------------------------------------------

        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);

        // --- I OVO JE BITNO ---
        glGenerateMipmap(GL_TEXTURE_2D);
        // ----------------------

        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

GLFWcursor* loadImageToCursor(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;

    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);

    if (ImageData != NULL)
    {
        GLFWimage image;
        image.width = TextureWidth;
        image.height = TextureHeight;
        image.pixels = ImageData;

        // Tacka na površini slike kursora koja se ponaša kao hitboks, moze se menjati po potrebi
        // Trenutno je gornji levi ugao, odnosno na 20% visine i 20% sirine slike kursora
        int hotspotX = TextureWidth / 5;
        int hotspotY = TextureHeight / 5;

        GLFWcursor* cursor = glfwCreateCursor(&image, hotspotX, hotspotY);
        stbi_image_free(ImageData);
        return cursor;
    }
    else {
        std::cout << "Kursor nije ucitan! Putanja kursora: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return nullptr;
    }
}

// Load 3D OBJ model using ModelCache
unsigned int loadOBJModel(const char* filepath, ModelCache& cache) {
    return cache.loadModel(filepath);
}

// Unified render function that handles both 2D quads and 3D models
void RenderObject3D(unsigned int shader, unsigned int quadVAO, GameObject& obj, 
                    Camera& camera, float aspectRatio, ModelCache& cache, int roundingMode) {
    if (!obj.isVisible) return;

    glUseProgram(shader);

    // Create model matrix (position, rotation, scale)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(obj.x, obj.y, obj.z));
    
    // Apply rotation if object has rotation values
    if (obj.rotateX != 0.0f) 
        model = glm::rotate(model, glm::radians(obj.rotateX), glm::vec3(1.0f, 0.0f, 0.0f));
    if (obj.rotateY != 0.0f) 
        model = glm::rotate(model, glm::radians(obj.rotateY), glm::vec3(0.0f, 1.0f, 0.0f));
    if (obj.rotateZ != 0.0f) 
        model = glm::rotate(model, glm::radians(obj.rotateZ), glm::vec3(0.0f, 0.0f, 1.0f));
    
    model = glm::scale(model, glm::vec3(obj.w, obj.h, obj.d));

    // Get view and projection matrices from camera
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);

    // Set matrix uniforms
    unsigned int uModelLoc = glGetUniformLocation(shader, "uModel");
    unsigned int uViewLoc = glGetUniformLocation(shader, "uView");
    unsigned int uProjLoc = glGetUniformLocation(shader, "uProjection");

    glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uViewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(uProjLoc, 1, GL_FALSE, glm::value_ptr(projection));

    unsigned int uColorLoc = glGetUniformLocation(shader, "uColor");
    unsigned int uUseTexLoc = glGetUniformLocation(shader, "uUseTexture");
    unsigned int uRoundingLoc = glGetUniformLocation(shader, "uRounding");

    glUniform4f(uColorLoc, obj.r, obj.g, obj.b, obj.a);
    glUniform1i(uRoundingLoc, roundingMode);

    if (obj.useTexture) {
        glUniform1i(uUseTexLoc, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, obj.textureId);
    }
    else {
        glUniform1i(uUseTexLoc, 0);
    }

    // Decide whether to use 3D model or 2D quad
    if (obj.is3DModel && obj.modelVAO != 0) {
        // Render 3D model
        glBindVertexArray(obj.modelVAO);
        
        // Get vertex count from the model
        Model* modelData = cache.getModel(obj.modelPath.c_str());
        if (modelData && modelData->vertexCount > 0) {
            glDrawArrays(GL_TRIANGLES, 0, modelData->vertexCount);
        }
        
        glBindVertexArray(0);
    }
    else {
        // Render 2D quad (backward compatible)
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
}

// Pass light uniforms to shader for Phong lighting
void setLightUniforms(unsigned int shader, const Light& light, const Camera& camera) {
    glUseProgram(shader);
    
    // Light position
    unsigned int lightPosLoc = glGetUniformLocation(shader, "uLightPos");
    glUniform3f(lightPosLoc, light.position.x, light.position.y, light.position.z);
    
    // Light color
    unsigned int lightColorLoc = glGetUniformLocation(shader, "uLightColor");
    glUniform3f(lightColorLoc, light.color.r, light.color.g, light.color.b);
    
    // Light strength
    unsigned int lightStrengthLoc = glGetUniformLocation(shader, "uLightStrength");
    glUniform1f(lightStrengthLoc, light.strength);
    
    // Light enabled/disabled
    unsigned int lightEnabledLoc = glGetUniformLocation(shader, "uLightEnabled");
    glUniform1i(lightEnabledLoc, light.enabled ? 1 : 0);
    
    // Camera position (for specular calculation)
    unsigned int viewPosLoc = glGetUniformLocation(shader, "uViewPos");
    glUniform3f(viewPosLoc, camera.position.x, camera.position.y, camera.position.z);
}