#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

// GLM includes for 3D math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Header/Util.h"
#include "../Header/Model.h"
#include "../Header/GameObject.h"
#include "../Header/Camera.h"

// --- KONSTANTE I STANJA ---
enum GameState {
    MENU,
    COOKING,
    ASSEMBLY,
    FINISHED
};

// GameObject and Camera are now defined in headers

enum IngredientType {
    SOLID,
    SAUCE
};

struct Ingredient {
    GameObject obj;
    IngredientType type;
    std::string name;
    bool placed;
};

const double TARGET_FPS = 75.0;
const double OPTIMAL_TIME = 1.0 / TARGET_FPS;

// --- POMOCNE FUNKCIJE ---

bool CheckCollision(GameObject& one, GameObject& two) {
    float oneLeft = one.x - one.w / 2;
    float oneRight = one.x + one.w / 2;
    float oneTop = one.y + one.h / 2;
    float oneBottom = one.y - one.h / 2;

    float twoLeft = two.x - two.w / 2;
    float twoRight = two.x + two.w / 2;
    float twoTop = two.y + two.h / 2;
    float twoBottom = two.y - two.h / 2;

    bool collisionX = oneRight >= twoLeft && twoRight >= oneLeft;
    bool collisionY = oneTop >= twoBottom && twoTop >= oneBottom;

    return collisionX && collisionY;
}

// Updated RenderObject to use 3D transformations
void RenderObject(unsigned int shader, unsigned int VAO, GameObject& obj, Camera& camera, float aspectRatio, int roundingMode = 0) {
    if (!obj.isVisible) return;

    glUseProgram(shader);

    // Create model matrix (position in 3D space, facing camera)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(obj.x, obj.y, obj.z));
    model = glm::scale(model, glm::vec3(obj.w, obj.h, obj.d));

    // Rotation transformations
    model = glm::rotate(model, glm::radians(obj.rotateX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(obj.rotateY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(obj.rotateZ), glm::vec3(0.0f, 0.0f, 1.0f));

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

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error: %s\n", description);
}

// Pomocna funkcija za crtanje krastavaca (posto su 4 dela) ili obicnog objekta
void DrawIngredientVisuals(unsigned int shader, unsigned int VAO, Ingredient& ing, Camera& camera, float aspectRatio) {
    int rounding = 0;
    if (ing.name == "BunBot") rounding = 1;
    if (ing.name == "BunTop") rounding = 2;

    if (ing.name == "Pickles") {
        // Iseckani krastavcici: Crtamo 4 mala dela umesto jednog velikog
        float totalWidth = ing.obj.w;
        float partWidth = totalWidth / 4.0f;
        float startX = ing.obj.x - (totalWidth / 2.0f) + (partWidth / 2.0f);

        for (int k = 0; k < 4; k++) {
            GameObject slice = ing.obj;
            slice.w = partWidth * 0.8f; // Malo razmaka izmedju njih
            slice.x = startX + k * partWidth;
            RenderObject(shader, VAO, slice, camera, aspectRatio, 0);
        }
    }
    else {
        // Obicno crtanje za sve ostalo
        RenderObject(shader, VAO, ing.obj, camera, aspectRatio, rounding);
    }
}

int main()
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return endProgram("GLFW nije uspeo da se inicijalizuje.");

    // --- IZMENE ZA CORE PROFILE 3.3 ---
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Opciono: potrebno za Mac OS
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Brza Hrana - Projekat", primaryMonitor, NULL);
    //GLFWwindow* window = glfwCreateWindow(800, 600, "Brza Hrana - Test", NULL, NULL);

    if (window == NULL) return endProgram("Prozor nije uspeo da se kreira.");
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE; // Obavezno za Core Profile
    if (glewInit() != GLEW_OK) return endProgram("GLEW nije uspeo da se inicijalizuje.");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    unsigned int shaderProgram = createShader("Shaders/basic.vert", "Shaders/basic.frag");

    // Updated vertices for 3D (vec3 positions + vec2 texcoords)
    float vertices[] = {
        // positions (x,y,z)   // texture coords
        -0.5f,  0.5f, 0.0f,    0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,
         0.5f,  0.5f, 0.0f,    1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,    1.0f, 0.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO); // Core profile zahteva VAO
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (location 0) - now vec3
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture coordinate attribute (location 1) - still vec2
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Odvajanje VAO (dobra praksa u Core profilu)
    glBindVertexArray(0);

    GLFWcursor* cursor = loadImageToCursor("Resources/cursor_spatula.png");
    if (cursor) glfwSetCursor(window, cursor);

    // --- CREATE CAMERA ---
    Camera camera;
    camera.position = glm::vec3(0.0f, 0.0f, 3.0f);  // Position camera in front of the scene
    camera.yaw = -90.0f;  // Look towards negative Z
    camera.pitch = 0.0f;
    camera.updateCameraVectors();

    unsigned int studentTex = loadImageToTexture("Resources/student_info.png");
    GameObject studentInfo;
    studentInfo.w = 0.5f; studentInfo.h = 0.3f;
    studentInfo.x = 0.7f; studentInfo.y = 0.8f;
    studentInfo.useTexture = (studentTex != 0);
    studentInfo.textureId = studentTex;
    studentInfo.a = 0.5f;
    if (!studentInfo.useTexture) { studentInfo.r = 0; studentInfo.g = 0; studentInfo.b = 0; }

    ModelCache modelCache;  // Create once at startup
    unsigned int vao = loadOBJModel("Models/gornja.obj", modelCache);
    GameObject obj3D;
    obj3D.is3DModel = true;
    obj3D.modelVAO = vao;
    obj3D.modelPath = "Models/gornja.obj";
    obj3D.x = 0.0f; obj3D.y = 0.0f; obj3D.z = -0.0f;
    obj3D.w = obj3D.h = obj3D.d = 0.2f; // Scale
    obj3D.rotateY = 45.0f; // Optional rotation

    


    // --- STATE PROMENLJIVE ---
    GameState currentState = MENU;

    GameObject btnOrder;
    btnOrder.w = 0.4f; btnOrder.h = 0.2f;
    btnOrder.r = 0.9f; btnOrder.g = 0.6f; btnOrder.b = 0.1f;

    GameObject stove;
    stove.y = -0.7f; stove.w = 1.2f; stove.h = 0.6f;
    stove.r = 0.2f; stove.g = 0.2f; stove.b = 0.2f;

    GameObject rawPatty;
    rawPatty.y = 0.5f; rawPatty.w = 0.3f; rawPatty.h = 0.2f;
    rawPatty.r = 0.9f; rawPatty.g = 0.6f; rawPatty.b = 0.6f;

    GameObject loadingBarBorder;
    loadingBarBorder.y = 0.9f; loadingBarBorder.w = 0.8f; loadingBarBorder.h = 0.1f;
    loadingBarBorder.r = 1.0f; loadingBarBorder.g = 1.0f; loadingBarBorder.b = 1.0f;

    GameObject loadingBarFill;
    loadingBarFill.y = 0.9f; loadingBarFill.h = 0.08f; loadingBarFill.w = 0.0f;
    loadingBarFill.r = 0.0f; loadingBarFill.g = 1.0f; loadingBarFill.b = 0.0f;

    float cookingProgress = 0.0f;

    GameObject table;
    table.y = -0.7f; table.w = 2.0f; table.h = 0.8f;
    table.r = 0.6f; table.g = 0.4f; table.b = 0.2f;

    GameObject plate;
    plate.y = -0.6f; plate.w = 0.6f; plate.h = 0.1f;
    plate.r = 1.0f; plate.g = 1.0f; plate.b = 1.0f;

    std::vector<Ingredient> ingredients;
    auto addIng = [&](std::string name, float r, float g, float b, IngredientType type = SOLID) {
        Ingredient ing;
        ing.name = name;
        ing.obj.w = (type == SAUCE) ? 0.1f : 0.3f;
        ing.obj.h = (type == SAUCE) ? 0.3f : 0.05f;
        ing.obj.r = r; ing.obj.g = g; ing.obj.b = b;
        ing.obj.y = 0.5f;
        ing.type = type;
        ing.placed = false;
        ingredients.push_back(ing);
        };

    addIng("BunBot", 0.8f, 0.6f, 0.2f);
    addIng("Patty", 0.5f, 0.25f, 0.0f);
    addIng("Ketchup", 1.0f, 0.0f, 0.0f, SAUCE);
    addIng("Mustard", 1.0f, 1.0f, 0.0f, SAUCE);
    addIng("Pickles", 0.0f, 0.5f, 0.0f);
    addIng("Onion", 0.9f, 0.9f, 0.9f);
    addIng("Lettuce", 0.0f, 1.0f, 0.0f);
    addIng("Cheese", 1.0f, 0.8f, 0.0f);
    addIng("Tomato", 1.0f, 0.0f, 0.0f);
    addIng("BunTop", 0.8f, 0.6f, 0.2f);

    int currentIngredientIndex = 0;
    std::vector<GameObject> puddles;

    GameObject endMessage;
    endMessage.w = 0.8f; endMessage.h = 0.4f;
    endMessage.useTexture = true;
    unsigned int msgTex = loadImageToTexture("Resources/prijatno.png");
    if (msgTex) endMessage.textureId = msgTex;
    else { endMessage.r = 0; endMessage.g = 0; endMessage.b = 1; endMessage.useTexture = false; }

    double lastTime = glfwGetTime();
    bool spacePressedLastFrame = false;

    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        if (now - lastTime < OPTIMAL_TIME) continue;
        float deltaTime = (float)(now - lastTime);
        lastTime = now;

        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // --- CAMERA CONTROLS ---
        bool allowCameraMovement = (currentState != MENU && currentState != FINISHED);

        // Arrow key camera movement
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            camera.processKeyboard(GLFW_KEY_UP, deltaTime, allowCameraMovement);
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            camera.processKeyboard(GLFW_KEY_DOWN, deltaTime, allowCameraMovement);
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            camera.processKeyboard(GLFW_KEY_LEFT, deltaTime, allowCameraMovement);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            camera.processKeyboard(GLFW_KEY_RIGHT, deltaTime, allowCameraMovement);

        // Mouse camera rotation (only when right mouse button is held)
        bool allowCameraRotation = allowCameraMovement && (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
        if (allowCameraRotation) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            camera.processMouseMovement(mouseX, mouseY, true);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get window size for aspect ratio
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        float aspectRatio = (float)windowWidth / (float)windowHeight;

        // --- RENDER LOGIKA ---
        
        RenderObject3D(shaderProgram, VAO, obj3D, camera, aspectRatio, modelCache);
        RenderObject(shaderProgram, VAO, studentInfo, camera, aspectRatio);

        if (currentState == MENU) {
            RenderObject(shaderProgram, VAO, btnOrder, camera, aspectRatio);
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

                double mx, my;
                glfwGetCursorPos(window, &mx, &my);
                int w, h;
                glfwGetWindowSize(window, &w, &h);
                float ndcX = (2.0f * mx) / w - 1.0f;
                float ndcY = 1.0f - (2.0f * my) / h;
                if (ndcX > (btnOrder.x - btnOrder.w / 2) && ndcX < (btnOrder.x + btnOrder.w / 2) &&
                    ndcY >(btnOrder.y - btnOrder.h / 2) && ndcY < (btnOrder.y + btnOrder.h / 2))
                {
                    currentState = COOKING;
                }
            }
        }
        else if (currentState == COOKING) {
            float speed = 1.0f * deltaTime;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) rawPatty.y += speed;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) rawPatty.y -= speed;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) rawPatty.x -= speed;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) rawPatty.x += speed;

            if (CheckCollision(rawPatty, stove)) {
                cookingProgress += 0.3f * deltaTime;
                if (cookingProgress > 1.0f) cookingProgress = 1.0f;
                rawPatty.r = 0.9f + (0.5f - 0.9f) * cookingProgress;
                rawPatty.g = 0.6f + (0.25f - 0.6f) * cookingProgress;
                rawPatty.b = 0.6f + (0.0f - 0.6f) * cookingProgress;
                loadingBarFill.w = 0.78f * cookingProgress;
            }

            RenderObject(shaderProgram, VAO, stove, camera, aspectRatio);
            RenderObject(shaderProgram, VAO, rawPatty, camera, aspectRatio);
            RenderObject(shaderProgram, VAO, loadingBarBorder, camera, aspectRatio);
            loadingBarFill.x = loadingBarBorder.x - loadingBarBorder.w / 2 + loadingBarFill.w / 2 + 0.01f;
            RenderObject(shaderProgram, VAO, loadingBarFill, camera, aspectRatio);

            if (cookingProgress >= 1.0f) currentState = ASSEMBLY;
        }
        else if (currentState == ASSEMBLY) {
            RenderObject(shaderProgram, VAO, table, camera, aspectRatio);
            RenderObject(shaderProgram, VAO, plate, camera, aspectRatio);

            for (auto& p : puddles) RenderObject(shaderProgram, VAO, p, camera, aspectRatio);

            // Crtanje naslaganih
            float stackHeight = plate.y + 0.05f;
            for (int i = 0; i < currentIngredientIndex; i++) {
                if (ingredients[i].type == SOLID) {
                    Ingredient tempIng = ingredients[i]; // Kopija za modifikaciju pozicije
                    tempIng.obj.y = stackHeight;
                    tempIng.obj.x = plate.x;
                    stackHeight += 0.06f;

                    // Pozivamo helper funkciju koja zna da nacrta krastavce ili zemicke
                    DrawIngredientVisuals(shaderProgram, VAO, tempIng, camera, aspectRatio);
                }
            }

            if (currentIngredientIndex < ingredients.size()) {
                Ingredient& curr = ingredients[currentIngredientIndex];

                float speed = 1.0f * deltaTime;
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) curr.obj.y += speed;
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) curr.obj.y -= speed;
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) curr.obj.x -= speed;
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) curr.obj.x += speed;

                // Crtamo trenutni sastojak (sa krastavac/zemicka logikom)
                DrawIngredientVisuals(shaderProgram, VAO, curr, camera, aspectRatio);

                if (curr.type == SAUCE) {
                    bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
                    if (spacePressed && !spacePressedLastFrame) {

                        float plateLeft = plate.x - plate.w / 2.0f;
                        float plateRight = plate.x + plate.w / 2.0f;

                        // Proveravamo da li je centar sosa unutar X granica tanjira
                        bool isAlignedWithPlate = (curr.obj.x > plateLeft && curr.obj.x < plateRight);

                        if (isAlignedWithPlate) {
                            // Uspeh
                            currentIngredientIndex++;
                        }
                        else {
                            // Barica na stolu
                            GameObject puddle;
                            puddle.x = curr.obj.x;
                            puddle.y = -0.65f;
                            puddle.w = 0.15f; puddle.h = 0.1f;
                            puddle.r = curr.obj.r; puddle.g = curr.obj.g; puddle.b = curr.obj.b;
                            puddles.push_back(puddle);
                        }
                    }
                    spacePressedLastFrame = spacePressed;
                }
                else { // SOLID
                    float dist = sqrt(pow(curr.obj.x - plate.x, 2) + pow(curr.obj.y - stackHeight, 2));
                    if (dist < 0.1f) {
                        currentIngredientIndex++;
                    }
                }
            }
            else {
                currentState = FINISHED;
            }
        }
        else if (currentState == FINISHED) {
            RenderObject(shaderProgram, VAO, table, camera, aspectRatio);
            RenderObject(shaderProgram, VAO, plate, camera, aspectRatio);
            float stackHeight = plate.y + 0.05f;
            for (auto& ing : ingredients) {
                if (ing.type == SOLID) {
                    Ingredient tempIng = ing;
                    tempIng.obj.x = plate.x;
                    tempIng.obj.y = stackHeight;
                    DrawIngredientVisuals(shaderProgram, VAO, tempIng, camera, aspectRatio);
                    stackHeight += 0.06f;
                }
            }
            RenderObject(shaderProgram, VAO, endMessage, camera, aspectRatio);
        }

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}