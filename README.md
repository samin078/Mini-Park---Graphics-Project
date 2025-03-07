# Mini-Park — Graphics Project

Mini-Park is a computer graphics project developed using OpenGL. It simulates a small amusement park with interactive elements, lighting effects, and animated rides. The project demonstrates 3D modeling, transformations, and real-time user interaction.

## Features
- **Camera System:** Move freely using keyboard controls with support for translation, rotation, and zoom.
- **Dynamic Objects:** Toggle and control amusement park rides like the Ferris wheel (Nagordola) and swing (Dolna).
- **Lighting Effects:** Multiple lighting options, including directional, point, and spotlight effects.
- **Textures & Shading:** Objects have textures and custom shaders for better visuals.
- **Transformations:** Objects can be translated, rotated, and scaled.
- **Fractal Tree Rendering:** Uses recursion to generate trees with textured branches and leaves.

## Controls

### Camera Movement
- `W` - Move forward  
- `S` - Move backward  
- `A` - Move left  
- `D` - Move right  
- `E` - Move up  
- `R` - Move down  
- `X` - Pitch up  
- `C` - Pitch down  
- `Y` - Yaw left  
- `V` - Yaw right  
- `Z` - Roll left  
- `Q` - Roll right  

### Lighting Controls
- `1` - Toggle directional light  
- `2` - Turn on all point lights  
- `3` - Turn off all point lights  
- `4` - Toggle spotlight  
- `5` - Turn on spotlight  
- `6` - Turn off spotlight  
- `7` - Enable diffuse lighting  
- `8` - Enable specular lighting  
- `9` - Enable ambient lighting  
- `0` - Enable specular lighting  

### Individual Light Controls
- `Numpad 1` - Turn on point light 1  
- `Numpad 2` - Turn off point light 1  
- `Numpad 3` - Turn on point light 3  
- `Numpad 4` - Turn off point light 3  
- `Numpad 5` - Turn on point light 4  
- `Numpad 6` - Turn off point light 4  
- `Numpad 7` - Turn on point light 5  
- `Numpad 8` - Turn off point light 5  

### Environment Toggles
- `O` - Set night mode (enable specific lights)  
- `P` - Disable night mode  

### Object Toggles
- `F` - Toggle Ferris wheel (Nagordola)  
- `G` - Toggle swing (Dolna)  

### Exit
- `Esc` - Close the application  

## Implementation Details
- Developed using **C++** with **OpenGL** and **GLFW**.
- Uses **GLM** for mathematical operations like transformations and camera movement.
- Implements **VAOs, VBOs, and shaders** for rendering objects.
- Fractal trees are generated using recursion for a natural look.

This project demonstrates key computer graphics concepts and real-time rendering using OpenGL.
