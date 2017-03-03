# Week2AIE
##**Landscape**  
This week contained work with landscapes.  
**What was acheived:**  
- Heightmaps  
- Normals  
- Textured surface  
- Diffuse Lighting  
- Diffuse Lighting with Normals  
**What i Added:**  
- Loading shader within seperate class  
- Loading in shader from file  
- Having all resources within Landscape.proj and have Pre-Build events copy over data for .tex etc.  
- Shader errors are thrown to console.  
**Using The UI:**
- You will be presented with the following:
    -Post Processing:
        -Enabling Post Processing will allow you to enable - Blur, Distortion and Grey Scale
        -This can be turned back of by unticking all the the boxes for these options
    -Lighting Editor
        -You can adjust the Ambient Strength and Speculat Strength by dragging a slider
        -Light and Specular colour can be adjusted by typing in a new colour code.
    -Landscape Editor
        -you can enable and disable wireframe mode for the world.
    -Debugger
        -This will show the camera and light position aswell as the FPS. 
    -Object Creator
        -You can type in the values for the postion and scale for an Object
        -Select a texture to be applied to it aswell as type of object.
        -Create Object Button will create an object and place it in the world with these values.
    -Animator
        -This will present you with the current frame of the animation
        -You can also select which animation to play
    -Particles
        -This will allow you to adjust the following and adjust it realtime;
            -Max Particles
            -Emit Rate
            -Lifetime Min
            -Lifetime Max
            -Velocity Min
            -Velocity Max
            -Start Size
            -End Size
            -Start Colour
            -End Colour
            -Position
    -Rest Particles will reset to a default state
