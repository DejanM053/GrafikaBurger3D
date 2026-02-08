# Sauce Bottle Zone Detection - Height-Independent System

## Overview
The sauce bottle zone detection system has been updated to detect zones based **only on X and Z positions**, ignoring the bottle's height (Y position). This makes it much easier for players to successfully place sauce on the burger.

## What Changed

### Previous System
- Used full 3D collision detection (`CheckCollision3D`)
- Required the bottle to be at a **specific height** to be detected in the correct zone
- Made it difficult to tell if you were above the plate, table, or floor

### New System
- Uses 2D collision detection (`CheckCollisionXZ`) for sauce bottles
- Only checks **X and Z positions** (horizontal plane)
- **Height (Y) is completely ignored** for zone detection
- Much easier to aim - as long as you're horizontally above the target, it will work

## How It Works

### Zone Priority System
When you press ENTER to squeeze the sauce bottle, the system checks zones in this order:

1. **Plate Zone** (highest priority)
   - If bottle's X/Z position is within plate boundaries ? Sauce goes ON burger
   - Moves to next ingredient

2. **Table Zone** (medium priority)
   - If bottle's X/Z position is within table boundaries ? Sauce spills on table
   - Creates splat, bottle resets, player tries again

3. **Floor Zone** (lowest priority)
   - If bottle's X/Z position is outside table ? Sauce spills on floor
   - Creates splat, bottle resets, player tries again

4. **No Zone**
   - If bottle is somehow outside all zones ? Nothing happens
   - Player can keep trying

### Visual Guide

```
Top View (looking down at the table):

                    FLOOR ZONE
    ??????????????????????????????????????
    ?                                    ?
    ?          TABLE ZONE                ?
    ?    ???????????????????????        ?
    ?    ?                     ?        ?
    ?    ?    PLATE ZONE       ?        ?
    ?    ?      [Plate]        ?        ?
    ?    ?     (Success!)      ?        ?
    ?    ?                     ?        ?
    ?    ???????????????????????        ?
    ?        (Spill on table)            ?
    ?                                    ?
    ??????????????????????????????????????
              (Spill on floor)

Only X and Z positions matter!
The bottle can be at ANY height above/below.
```

## Adjusting Zone Sizes

### Easy Adjustment Constants
All zone sizes are defined in **one central location** with clear comments:

```cpp
// Location: Around line 340 in Main.cpp

// ========================================
// SAUCE BOTTLE ZONE SIZE ADJUSTMENTS
// ========================================

// PLATE ZONE - Sauce goes on burger
const float PLATE_ZONE_X = 0.0f;       // Center X position
const float PLATE_ZONE_Z = 0.0f;       // Center Z position
const float PLATE_ZONE_WIDTH = 0.5f;   // Width (X axis) - ADJUST THIS
const float PLATE_ZONE_DEPTH = 0.5f;   // Depth (Z axis) - ADJUST THIS

// TABLE ZONE - Sauce spills on table
const float TABLE_ZONE_X = 0.0f;       // Center X position
const float TABLE_ZONE_Z = 0.0f;       // Center Z position
const float TABLE_ZONE_WIDTH = 2.0f;   // Width (X axis) - ADJUST THIS
const float TABLE_ZONE_DEPTH = 2.0f;   // Depth (Z axis) - ADJUST THIS

// FLOOR ZONE - Sauce spills on floor
const float FLOOR_ZONE_X = 0.0f;       // Center X position
const float FLOOR_ZONE_Z = 0.0f;       // Center Z position
const float FLOOR_ZONE_WIDTH = 10.0f;  // Width (X axis) - ADJUST THIS
const float FLOOR_ZONE_DEPTH = 10.0f;  // Depth (Z axis) - ADJUST THIS
```

### How to Adjust

#### Make Plate Zone Bigger (easier to hit)
```cpp
const float PLATE_ZONE_WIDTH = 0.7f;   // Increased from 0.5f
const float PLATE_ZONE_DEPTH = 0.7f;   // Increased from 0.5f
```

#### Make Plate Zone Smaller (harder to hit, more realistic)
```cpp
const float PLATE_ZONE_WIDTH = 0.3f;   // Decreased from 0.5f
const float PLATE_ZONE_DEPTH = 0.3f;   // Decreased from 0.5f
```

#### Move Plate Zone
```cpp
const float PLATE_ZONE_X = 0.2f;       // Move right
const float PLATE_ZONE_Z = -0.1f;      // Move forward
```

#### Adjust Table Zone
```cpp
const float TABLE_ZONE_WIDTH = 2.5f;   // Wider table
const float TABLE_ZONE_DEPTH = 2.5f;   // Deeper table
```

## Testing Your Adjustments

### Step 1: Visual Test
1. Move bottle around in X and Z directions (W/A/S/D)
2. Press ENTER at different horizontal positions
3. Observe where sauce is placed:
   - On burger? ? You're in plate zone
   - On table? ? You're in table zone
   - On floor? ? You're in floor zone

### Step 2: Verify Zone Boundaries
```
Move bottle to these positions and press ENTER:

Position (X, Z)  | Expected Result
-----------------|-----------------
(0.0, 0.0)       | Sauce on burger (center of plate)
(0.3, 0.3)       | Sauce on burger (near plate edge)
(0.5, 0.5)       | Splat on table (outside plate)
(1.5, 1.5)       | Splat on table (far on table)
(3.0, 3.0)       | Splat on floor (off table)
```

### Step 3: Fine-Tune
- If plate zone is **too small** ? Increase WIDTH and DEPTH
- If plate zone is **too large** ? Decrease WIDTH and DEPTH
- If zones don't align with visuals ? Adjust X and Z center positions

## Implementation Details

### New Function: `CheckCollisionXZ`
```cpp
bool CheckCollisionXZ(GameObject& one, GameObject& two) {
    // Only checks X and Z axes, ignores Y (height)
    float oneHalfW = one.w / 2.0f;
    float oneHalfD = one.d / 2.0f;
    
    float twoHalfW = two.w / 2.0f;
    float twoHalfD = two.d / 2.0f;
    
    bool collisionX = (one.x + oneHalfW >= two.x - twoHalfW) && 
                      (one.x - oneHalfW <= two.x + twoHalfW);
    
    bool collisionZ = (one.z + oneHalfD >= two.z - twoHalfD) && 
                      (one.z - oneHalfD <= two.z + twoHalfD);
    
    return collisionX && collisionZ;
}
```

### Modified Sauce Detection Logic
```cpp
// Check zones using XZ-only collision (height doesn't matter)
if (CheckCollisionXZ(curr.obj, plateZone)) {
    // Place sauce on burger
}
else if (CheckCollisionXZ(curr.obj, tableZone)) {
    // Spill on table
}
else if (CheckCollisionXZ(curr.obj, floorZone)) {
    // Spill on floor
}
```

## Advantages of This System

### For Players
? **No need to worry about height** - just move horizontally  
? **Clear target area** - X and Z positions are easier to visualize  
? **Instant feedback** - Press ENTER to test any position  
? **Forgiving** - Height limitations handled separately (min height)  

### For Developers
? **Easy to adjust** - All constants in one place  
? **Visual debugging** - Can temporarily make zones visible  
? **Independent from Y-axis** - Height management stays separate  
? **Clear priority system** - Plate > Table > Floor  

## Debugging Tips

### Make Zones Visible
Temporarily make zones visible to see exactly where they are:

```cpp
// In initialization section
plateZone.isVisible = true;
plateZone.r = 0.0f;  // Green
plateZone.g = 1.0f;
plateZone.b = 0.0f;
plateZone.a = 0.3f;  // Semi-transparent

tableZone.isVisible = true;
tableZone.r = 1.0f;  // Red
tableZone.g = 0.0f;
tableZone.b = 0.0f;
tableZone.a = 0.2f;

// Don't forget to render them in ASSEMBLY state:
RenderObject(shaderProgram, VAO, plateZone, camera, aspectRatio);
RenderObject(shaderProgram, VAO, tableZone, camera, aspectRatio);
```

This will show:
- **Green box** = Plate zone (success area)
- **Red box** = Table zone (fail area)

### Print Debug Info
Add console output to see which zone is detected:

```cpp
if (CheckCollisionXZ(curr.obj, plateZone)) {
    std::cout << "PLATE ZONE - Success!" << std::endl;
    // ...
}
else if (CheckCollisionXZ(curr.obj, tableZone)) {
    std::cout << "TABLE ZONE - Spill!" << std::endl;
    // ...
}
```

## Common Scenarios

### Scenario 1: "Plate zone too small"
**Problem:** Hard to place sauce successfully  
**Solution:**
```cpp
const float PLATE_ZONE_WIDTH = 0.8f;   // Increase
const float PLATE_ZONE_DEPTH = 0.8f;   // Increase
```

### Scenario 2: "Table zone overlaps plate"
**Problem:** Sometimes creates splats when over plate  
**Solution:** This should NOT happen with the priority system. Plate is checked first.  
**If it does happen:** Check that plate zone constants are being used correctly.

### Scenario 3: "Zones don't match visual models"
**Problem:** Sauce placed in wrong spot visually  
**Solution:** Adjust center positions to match visual table/plate:
```cpp
const float PLATE_ZONE_X = 0.0f;   // Adjust to match plate visual center
const float PLATE_ZONE_Z = 0.0f;   // Adjust to match plate visual center
const float TABLE_ZONE_X = 0.0f;   // Adjust to match table visual center
const float TABLE_ZONE_Z = 0.0f;   // Adjust to match table visual center
```

### Scenario 4: "Want to require more precision"
**Problem:** Game is too easy  
**Solution:** Make plate zone smaller, require player to be more accurate:
```cpp
const float PLATE_ZONE_WIDTH = 0.3f;   // Much smaller
const float PLATE_ZONE_DEPTH = 0.3f;   // Much smaller
```

## Quick Reference Table

| Zone | Purpose | Default Size (Width × Depth) | Recommended Range |
|------|---------|------------------------------|-------------------|
| Plate | Success area | 0.5 × 0.5 | 0.3 - 0.8 |
| Table | Fail area | 2.0 × 2.0 | 1.5 - 3.0 |
| Floor | Fallback | 10.0 × 10.0 | 5.0 - 20.0 |

## Summary

**Key Improvement:** Sauce bottle zone detection now **ignores height (Y)** and only checks **horizontal position (X, Z)**.

**Result:**
- ? Much easier to aim
- ? Clear target area
- ? Height is only limited by minHeight constraint (prevents going through floor)
- ? All zone sizes easily adjustable in one central location
- ? Priority system ensures correct behavior (Plate > Table > Floor)

**To adjust zones:** Edit the constants around line 340 in `Main.cpp`
