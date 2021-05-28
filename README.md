# Final Project - Cynthia Zhang

## Controls
Touchpad Controls
* two-finger scroll - look around

Keyboard Controls
* Movement
    * A - strafe left
    * D - strafe right
    * S - dolly backward
    * W - dolly forward
* Cinematic Tour
    * G - start or pause tour
    * L - toggle "fixed tour" (can only toggle when not in tour); when "fixed tour" is off, the user can look around during the tour
* Lighting
    * Q - increase light angle (light travels in -x direction)
    * E - decrease light angle (light travels in +x direction)
* Debugging
    * Z - draw mesh 
    * P - print current eye location in world space

## Program Features
* Skybox
* Texture-mapped meshes (terrain, stable) and from .mtl files (horse, grass)
* Height-mapped terrain from a DEM (/resources/canyon/diffuse.png)
* Semi-randomized placement of grass on top of terrain

## Resources 
Skybox image: https://hdrihaven.com/hdri/?h=kiara_1_dawn \
Cubemap generator: https://jaxry.github.io/panorama-to-cubemap/ \
Terrain: https://www.cgtrader.com/free-3d-models/exterior/landscape/high-poly-canyon \
Stable: https://free3d.com/3d-model/rusticsmallhorsestable-2010-v1--153472.html \
Horse: https://www.cgtrader.com/free-3d-models/animals/mammal/realtime-horse \
Grass: https://www.cgtrader.com/free-3d-models/plant/grass/grass-lowpoly 

## References
Texture image with alpha values: https://learnopengl.com/Advanced-OpenGL/Blending \
Terrain from heightmap: https://youtu.be/O9v6olrHPwI