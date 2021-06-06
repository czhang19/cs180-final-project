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
Grass: https://www.cgtrader.com/free-3d-models/plant/grass/grass-lowpoly \
Bow and Arrow: https://www.cgtrader.com/free-3d-models/military/other/wooden-bow-and-arrow

## Testers
Horse 2: https://www.cgtrader.com/free-3d-models/animals/other/horse-b0fa928c-b67b-43cd-a003-7ea4dc40da77
Horse 3: http://users.csc.calpoly.edu/~zwood/teaching/csc471/finalS19/jeempey/index.html

## References
Jillian Empey's S19 Horse obj and animation frames: http://users.csc.calpoly.edu/~zwood/teaching/csc471/finalS19/jeempey/index.html \
Texture image with alpha values: https://learnopengl.com/Advanced-OpenGL/Blending \
Terrain from heightmap: https://youtu.be/O9v6olrHPwI \
Getting heights of terrain: https://youtu.be/6E2zjfzMs7c
Horse movement/orientation: https://youtu.be/d-kuzyCkjoQ

## Features to Add
- [ ] Animate horse movement
    - [ ] Add complete horse path through terrain
    - [x] Horse path should follow height of terrain
    - [ ] Orient horse normals
    - [x] Horse faces the direction it's moving in
    - [ ] Shade horse material
- [ ] Animate dummy
    - [x] Draw dummy sitting on horse
    - [ ] Animate dummy shooting arrow
- [ ] Add physics-based motion (calculate position + velocity)
    - [x] Animate arrow with velocity
    - [x] Make quiver with many arrows
    - [x] Load arrow when it hits the ground
- [x] Add particle effects
- [ ] Add game menu (start, retry)
- [ ] Add score on screen
- [ ] Make website

Past Graphics Proj Examples: http://users.csc.calpoly.edu/~zwood/teaching/teaching.html

## Issues
1. How to make particles stay same size in world space? 
2. How to use rotations to orient my arrow? How to make arrow more visible (add trail)? 
3. How to put text on the screen? 

## Notes
1. Scale model matrix for particles in shader? 