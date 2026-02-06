# 3D Model Loading - Implementation Guide

## Overview
I've successfully implemented 3D model loading functionality for your project. The system supports loading OBJ files and rendering them alongside your existing 2D quads.

## Files Created

### Header Files:
- **Header/Model.h** - Defines the Model structure and ModelCache class for managing 3D models
- **Header/GameObject.h** - Extended GameObject structure with 3D support (z position, depth, rotation, model VAO)
- **Header/Camera.h** - Separated Camera structure for better code organization

### Source Files:
- **Source/Model.cpp** - Implementation of OBJ file loading and ModelCache management

### Updated Files:
- **Header/Util.h** - Added `loadOBJModel()` and `RenderObject3D()` function declarations
- **Source/Util.cpp** - Implemented the new 3D rendering functions
- **Source/Main.cpp** - Updated to use new headers (GameObject and Camera now defined in headers)

### Example Assets:
- **Models/cube.obj** - A simple cube model for testing (this is a DATA file, not a source file)

## What You Need to Do Manually

### **IMPORTANT:** Exclude OBJ files from compilation

If you get a build error about `Models/cube.obj`, you need to exclude it from the build:
1. Open Visual Studio
2. In Solution Explorer, find `Models/cube.obj`
3. Right-click on it and select "Properties"
4. Set "Excluded From Build" to "Yes"
5. Click OK

Or simply delete the OBJ file from the Solution Explorer (it will still be on disk).

### Add Source Files to Your Project:

1. **Add Model.cpp to your project:**
   - Open Visual Studio
   - Right-click on "Source Files" in Solution Explorer
   - Choose "Add > Existing Item"
   - Select `Source/Model.cpp`

2. **Add the new header files to your project:**
   - Right-click on "Header Files"
   - Choose "Add > Existing Item"
   - Select `Header/Model.h`, `Header/GameObject.h`, and `Header/Camera.h`

3. **Create a Models folder** (optional):
   - Create a `Models/` directory in your project root
   - Place your OBJ files there
   - **DO NOT add OBJ files to the Visual Studio project** - they are data files, not source files

4. **Export OBJ models:**
   - Use Blender, Maya, or any 3D software
   - Export as OBJ with:
     - Triangulate faces enabled
     - Include normals
     - Include texture coordinates (if using textures)
     - Use Y-up axis

## New Features

### 1. GameObject Structure Extensions
The GameObject struct now includes:
```cpp
float z;                    // Z position for 3D
float d;                    // Depth dimension
float rotateX, rotateY, rotateZ;  // Rotation angles in degrees
bool is3DModel;             // Flag to use 3D model instead of quad
unsigned int modelVAO;      // VAO handle for the model
std::string modelPath;      // Path for cache lookup
```

### 2. Model Loading
```cpp
// Create a ModelCache instance (typically once at startup)
ModelCache modelCache;

// Load a 3D model
unsigned int vao = loadOBJModel("path/to/model.obj", modelCache);

// Assign to a GameObject
GameObject myObject;
myObject.is3DModel = true;
myObject.modelVAO = vao;
myObject.modelPath = "path/to/model.obj";
myObject.x = 0.0f;
myObject.y = 0.0f;
myObject.z = -2.0f;
myObject.w = 1.0f;  // scale X
myObject.h = 1.0f;  // scale Y
myObject.d = 1.0f;  // scale Z
myObject.rotateY = 45.0f;  // Rotate 45 degrees around Y axis
```

### 3. Rendering
```cpp
// The RenderObject3D function handles both 2D quads and 3D models
RenderObject3D(shaderProgram, quadVAO, myObject, camera, aspectRatio, modelCache);

// For backward compatibility, objects without is3DModel=true will render as 2D quads
```

## OBJ File Format Support

The OBJ loader supports:
- Vertex positions (v)
- Texture coordinates (vt)
- Normal vectors (vn)
- Triangular faces (f)

### Supported face formats:
- `f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3` (full format)
- `f v1//vn1 v2//vn2 v3//vn3` (position and normal only)
- `f v1/vt1 v2/vt2 v3/vt3` (position and texture only)
- `f v1 v2 v3` (position only)

**Note:** Only triangular faces are currently supported. Quad faces should be triangulated before loading.

## Usage Example

### In your main() function:

```cpp
int main() {
    // ... existing initialization code ...
    
    // Create model cache
    ModelCache modelCache;
    
    // Load a 3D model (e.g., a burger bun)
    unsigned int bunModelVAO = loadOBJModel("Models/bun.obj", modelCache);
    
    // Create a GameObject for the 3D model
    GameObject bun3D;
    bun3D.is3DModel = true;
    bun3D.modelVAO = bunModelVAO;
    bun3D.modelPath = "Models/bun.obj";
    bun3D.x = 0.0f;
    bun3D.y = 0.5f;
    bun3D.z = 0.0f;
    bun3D.w = 0.5f;  // Scale
    bun3D.h = 0.5f;
    bun3D.d = 0.5f;
    bun3D.r = 0.8f;  // Color tint
    bun3D.g = 0.6f;
    bun3D.b = 0.2f;
    
    // In your render loop:
    while (!glfwWindowShouldClose(window)) {
        // ... existing code ...
        
        // Render 3D model
        RenderObject3D(shaderProgram, VAO, bun3D, camera, aspectRatio, modelCache);
        
        // Existing 2D objects still work with RenderObject()
        RenderObject(shaderProgram, VAO, plate, camera, aspectRatio);
    }
    
    // Cleanup is automatic when modelCache goes out of scope
    return 0;
}
```

## Model Cache

The `ModelCache` class prevents loading the same model multiple times:
- Models are cached by filepath
- Automatically cleans up VAO/VBO on destruction
- `getModel()` retrieves cached model data
- `hasModel()` checks if a model is already loaded

## Shader Compatibility

The existing shaders are already compatible with 3D models:
- Vertex shader receives vec3 positions and vec3 normals
- Fragment shader applies basic lighting based on normals
- All matrix transformations (Model, View, Projection) are properly set

## Testing the Implementation

To verify everything works:

1. Create or download a simple OBJ file (e.g., a cube or sphere)
2. Place it in your project directory
3. Add this code after creating the camera:

```cpp
ModelCache modelCache;
unsigned int testModel = loadOBJModel("test.obj", modelCache);

GameObject test3D;
test3D.is3DModel = true;
test3D.modelVAO = testModel;
test3D.modelPath = "test.obj";
test3D.x = 0.0f;
test3D.y = 0.0f;
test3D.z = -2.0f;
test3D.w = 0.5f;
test3D.h = 0.5f;
test3D.d = 0.5f;
test3D.rotateY = 45.0f;  // Optional rotation

// In render loop:
RenderObject3D(shaderProgram, VAO, test3D, camera, aspectRatio, modelCache);
```

## Notes

- The z-axis points out of the screen (positive z comes towards you)
- Rotation angles are in degrees
- The existing `RenderObject()` function still works for 2D quads
- The new `RenderObject3D()` function works for both 2D and 3D objects
- Model caching prevents duplicate loading and saves memory
- The fragment shader applies basic diffuse lighting to 3D models

Your existing game code continues to work without modification since GameObject's constructor initializes all new fields to appropriate defaults (is3DModel=false, z=0, etc.).
