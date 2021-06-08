# Horseback Archery - Cynthia Zhang

## Description
I created a horseback archery game, where the player shoots targets while horseback riding. The horse gallops automatically on a fixed path through the terrain, while the player can look around and aim by touchpad scrolling. There are 20 targets total, and at the end of the course, the game displays the number of targets hit, balls shot, and accuracy.

## [Demo Video](https://youtu.be/VqhD6X1noTE)

## Technologies
* Hierarchically-modeled and keyframed animation
* Physics-based animation - projectile motion
* Collision detection
* Particle systems
* Third-person follow camera
* Height-mapped terrain from a DEM
* Texture-mapped meshes from mtl files 
* Blinn-Phong Shading with directional light
* Text Rendering 
* Skybox

## Details

The horse and character are both hierarchically modeled. The horse is animated with keyframes, as well as with global translations and rotations for more natural up-down motion. The horse always stays at the height of the ground, faces the direction it's moving, and gallops smoothly by following a Bezier curve. 

I implemented the arrows as balls, because arrows were difficult to visually track after being fired and creating an arrow trail was outside the scope of my project. The balls follow projectile motion after being fired and return to the player's quiver upon contact with either a target, the ground, or the skybox (out of bounds). The player can have at most 10 balls in the air at a time. For each additional ball launched, the first-launched ball in the air is reused and counts as a "missed" shot towards the player's accuracy. 

There is collision detection between the balls and the targets, using sphere vs AABB (axis aligned bounding box) collision detection. Upon collision, the target explodes into a particle system and the ball returns to the player's quiver. 

The game camera stays at a fixed radius around the horse with vertical constraints. It is controlled with pitch and yaw by the player's touchpad scroll. The center of the sphere is a point above the horse at all times. 

The entire terrain is generated from a DEM (digital elevation model), with an accompanying texture file. Although my DEM had a very high resolution of 4096x4096, I had trouble passing all these pixel values to the GPU in an array, so I instead used every 16th pixel, essentially converting my heightmap image to 256x256. Then for object placement on my terrain, I used barycentric coordinates to retrieve the terrain height anywhere in my world.   

Using the FreeType library, I implemented text rendering to display game instructions, stats and the final score. I also used a text-rendered plus sign for the crosshairs. 

## Controls
* Two-finger Touchpad Scroll - look around / aim
* SPACEBAR - shoot ball / begin or restart game
* Z - draw mesh 
* ESC - close game


## Resources
Skybox [image](https://hdrihaven.com/hdri/?h=kiara_1_dawn) and [cubemap generator](https://jaxry.github.io/panorama-to-cubemap/) (for skybox faces) \
3D OBJ models from CGtrader, TurboSquid and Free3D: [terrain](https://www.cgtrader.com/free-3d-models/exterior/landscape/high-poly-canyon), [stable](https://free3d.com/3d-model/rusticsmallhorsestable-2010-v1--153472.html), [grass](https://www.cgtrader.com/free-3d-models/plant/grass/grass-lowpoly), [bow and arrow](https://www.cgtrader.com/free-3d-models/military/other/wooden-bow-and-arrow), [target](https://www.turbosquid.com/3d-models/3d-archery-model-1552530) \
3D Horse and Animation frames from Jillian Empey's [website](http://users.csc.calpoly.edu/~zwood/teaching/csc471/finalS19/jeempey/index.html)

## References and Tutorials
Learn OpenGL tutorials: [blending](https://learnopengl.com/Advanced-OpenGL/Blending) and [text rendering](https://learnopengl.com/In-Practice/Text-Rendering) \
Thin Matrix OpenGL 3D Java Game Tutorials: [terrain heightmaps](https://youtu.be/O9v6olrHPwI), [terrain collision detection](https://youtu.be/6E2zjfzMs7c), [player movement and orientation](https://youtu.be/d-kuzyCkjoQ) \
CPE 476 S20 Finding Dory (text rendering example): [github repo](https://github.com/gmonteir/FishFinder) \
Collision detection with AABB: [tutorial](https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection)