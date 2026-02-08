# Manual Ingredient Height Adjustment Guide

## Overview
This guide explains how to manually adjust ingredient heights to make them sit properly on the burger without floating or having gaps.

## Fixed Issues
1. ? Sauce bottles now reset when you miss (you can keep trying)
2. ? Sauce bottles only advance to next ingredient on successful placement
3. ? All ingredients significantly lowered
4. ? Stack starts much closer to plate (0.02f instead of 0.05f)
5. ? Ingredient starting position lowered from 1.5f to 0.5f

## Key Parameters to Adjust

### 1. **Plate Zone Y Position** (Line ~357)
This is where ingredients snap to - the "base" of your burger.

```cpp
plateZone.y = -0.46f;   // ? ADJUST THIS if burger floats or sinks into plate
```

**How to adjust:**
- **Too high** (floating above plate) ? Decrease value (e.g., -0.48f, -0.50f)
- **Too low** (sinking into plate) ? Increase value (e.g., -0.44f, -0.42f)

### 2. **Stack Starting Offset** (Lines ~572, ~583, ~707)
How far above the plate zone the first ingredient starts.

```cpp
float stackHeight = plateZone.y + 0.02f;  // ? ADJUST THIS (currently 0.02f)
```

**Current value:** `0.02f` (very close to plate)

**How to adjust:**
- **Ingredients sinking into plate** ? Increase (e.g., 0.03f, 0.04f)
- **Gap between plate and bottom bun** ? Decrease (e.g., 0.01f, 0.005f)

**Important:** Change this in **THREE** places:
1. Line ~572: Stack height calculation
2. Line ~583: Render stacked ingredients loop
3. Line ~707: FINISHED state rendering

### 3. **Individual Ingredient Heights** (Lines ~423-432)

Each ingredient has a `stackSnapHeight` value that determines how much height it adds to the stack.

```cpp
// Format: name, VAO, modelPath, r, g, b, type, minHeight, stackSnapHeight
//                                                              ? THIS VALUE

addIngredient3D("BunBot",    ..., 0.04f);   // Bottom bun
addIngredient3D("Patty",     ..., 0.015f);  // Patty
addIngredient3D("Ketchup",   ..., 0.0f);    // Bottle (doesn't stack)
addIngredient3D("Mustard",   ..., 0.0f);    // Bottle (doesn't stack)
addIngredient3D("Pickles",   ..., 0.01f);   // Pickles
addIngredient3D("Onion",     ..., 0.01f);   // Onion
addIngredient3D("Lettuce",   ..., 0.015f);  // Lettuce
addIngredient3D("Cheese",    ..., 0.005f);  // Cheese (thinnest!)
addIngredient3D("Tomato",    ..., 0.015f);  // Tomato
addIngredient3D("BunTop",    ..., 0.04f);   // Top bun
```

**How to adjust each ingredient:**
- **Ingredients floating/have gaps** ? DECREASE these values
- **Ingredients overlapping** ? INCREASE these values

### 4. **Sauce Layer Height** (Line ~657)
When sauce is placed successfully, this controls its thickness.

```cpp
ingredients[currentIngredientIndex].stackSnapHeight = 0.005f;  // ? ADJUST (very thin)
```

**Current value:** `0.005f` (extremely thin layer)

**How to adjust:**
- **Too thick** ? Decrease (e.g., 0.003f, 0.001f)
- **Invisible** ? Increase (e.g., 0.008f, 0.01f)

### 5. **Ingredient Starting Height** (Line ~410)
Where ingredients appear when you start placing them.

```cpp
ing.obj.y = 0.5f;  // ? ADJUST THIS (start position above plate)
```

**Current value:** `0.5f` (lowered from 1.5f)

**How to adjust:**
- **Start too high** (hard to see) ? Decrease (e.g., 0.3f, 0.2f)
- **Start too low** (already at plate) ? Increase (e.g., 0.7f, 0.8f)

## Step-by-Step Adjustment Process

### Step 1: Get the Plate Right
1. Build the game
2. Go to ASSEMBLY state
3. Place bottom bun
4. Observe if it's **floating** or **sinking**
5. Adjust `plateZone.y` accordingly
6. Repeat until bottom bun sits perfectly on plate

### Step 2: Check Bottom Bun Height
1. With bottom bun placed, check the gap between plate and bun
2. If there's a gap: Decrease `plateZone.y + 0.02f` offset
3. If bun sinks: Increase the offset
4. The `0.02f` value should match in all 3 locations

### Step 3: Adjust Individual Ingredient Heights
1. Place each ingredient one by one
2. For each ingredient that **floats** or has a **gap**:
   ```cpp
   // Find the ingredient in lines 423-432 and REDUCE its height
   addIngredient3D("Ingredient", ..., 0.01f);  // Was 0.02f, now 0.01f
   ```
3. For each ingredient that **overlaps** or **sinks**:
   ```cpp
   // INCREASE its height
   addIngredient3D("Ingredient", ..., 0.03f);  // Was 0.02f, now 0.03f
   ```

### Step 4: Fine-Tune Sauce Layers
1. Place ketchup bottle successfully
2. Check if ketchup layer is visible
3. Adjust the `0.005f` value in line ~657
4. Repeat for mustard

## Quick Reference Table

### Typical Height Values (based on real burger proportions)

| Ingredient | Typical Range | Current Value | Notes |
|------------|---------------|---------------|-------|
| Bottom Bun | 0.03f - 0.05f | 0.04f | Base of burger |
| Patty | 0.01f - 0.02f | 0.015f | Main protein |
| Ketchup Layer | 0.003f - 0.008f | 0.005f | Very thin |
| Mustard Layer | 0.003f - 0.008f | 0.005f | Very thin |
| Pickles | 0.008f - 0.015f | 0.01f | Thin slices |
| Onion | 0.008f - 0.015f | 0.01f | Thin slices |
| Lettuce | 0.01f - 0.02f | 0.015f | Leafy, thicker |
| Cheese | 0.003f - 0.008f | 0.005f | Thin slice |
| Tomato | 0.01f - 0.02f | 0.015f | Medium slice |
| Top Bun | 0.03f - 0.05f | 0.04f | Top of burger |

## Making Everything Lower

If the entire burger is **floating above the plate**, adjust these in order:

1. **Lower `plateZone.y`**
   ```cpp
   plateZone.y = -0.50f;  // Was -0.46f
   ```

2. **Reduce starting offset**
   ```cpp
   float stackHeight = plateZone.y + 0.01f;  // Was 0.02f
   ```

3. **Lower all individual heights by 20-30%**
   ```cpp
   addIngredient3D("Patty", ..., 0.01f);    // Was 0.015f
   addIngredient3D("Lettuce", ..., 0.01f);  // Was 0.015f
   // etc.
   ```

## Making Everything Closer Together

If ingredients have **gaps** between them:

1. **Reduce all `stackSnapHeight` values proportionally**
   ```cpp
   // Multiply all heights by 0.5 (cut in half)
   addIngredient3D("BunBot", ..., 0.02f);   // Was 0.04f
   addIngredient3D("Patty", ..., 0.0075f);  // Was 0.015f
   addIngredient3D("Pickles", ..., 0.005f); // Was 0.01f
   // etc.
   ```

2. **Keep relative proportions**
   - Buns should be thickest (0.03f - 0.05f)
   - Patty medium (0.01f - 0.02f)
   - Vegetables thin (0.008f - 0.015f)
   - Cheese/Sauces thinnest (0.003f - 0.008f)

## Testing Checklist

After each adjustment:
- [ ] Bottom bun sits flat on plate (not floating, not sinking)
- [ ] No gaps visible between ingredients
- [ ] No ingredients overlapping/clipping
- [ ] Patty clearly visible
- [ ] Ketchup/mustard layers visible but thin
- [ ] Vegetables sit naturally
- [ ] Top bun sits on top without floating
- [ ] Entire burger looks proportional

## Common Fixes

### "Burger floating way above plate"
**Solution:**
```cpp
plateZone.y = -0.55f;  // Make MORE negative
```

### "Ingredients have gaps between them"
**Solution:** Reduce ALL `stackSnapHeight` values by 30-50%

### "Everything is overlapping"
**Solution:** Increase ALL `stackSnapHeight` values by 50-100%

### "Sauce layers invisible"
**Solution:**
```cpp
ingredients[currentIngredientIndex].stackSnapHeight = 0.01f;  // Increase from 0.005f
```

### "Patty sinking into bun"
**Solution:**
```cpp
addIngredient3D("Patty", ..., 0.02f);  // Increase from 0.015f
```

## Advanced: Using Visual Debugging

Temporarily make plateZone visible to see where ingredients should snap:

```cpp
// In initialization (around line 357)
plateZone.isVisible = true;
plateZone.r = 0.0f;
plateZone.g = 1.0f;
plateZone.b = 0.0f;
plateZone.a = 0.3f;

// In ASSEMBLY state rendering (after line 570)
RenderObject(shaderProgram, VAO, plateZone, camera, aspectRatio);
```

This shows a **green semi-transparent box** where ingredients should land. If the box is:
- **Too high** ? Lower `plateZone.y`
- **Too low** ? Raise `plateZone.y`
- **Perfect** ? Remove the debug rendering

## Current Configuration Summary

**Plate Base:** `plateZone.y = -0.46f`  
**Stack Offset:** `0.02f`  
**Starting Position:** `0.5f`  

**Heights:**
- Buns: `0.04f` (thick)
- Patty: `0.015f` (medium)
- Vegetables: `0.01f` - `0.015f` (thin to medium)
- Cheese: `0.005f` (very thin)
- Sauces: `0.005f` (very thin)

All values are significantly lower than before, creating a compact, realistic burger!

## Sauce Bottle Behavior (Fixed)

**Before:** Bottle disappeared on failed squeeze, floating bottle remained  
**After:**
- ? Successful squeeze (above burger) ? Sauce placed, next ingredient
- ? Failed squeeze (above table/floor) ? Splat created, bottle **resets position**, player can try again
- ? Squeeze in air ? Nothing happens, player can try again

The bottle will never advance to the next ingredient until you successfully place sauce on the burger!
