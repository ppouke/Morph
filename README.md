# Morph

Morph a mesh based on vec3 data in the form (row, column, value).  
The application procedurally creates a 20x20 grid and reads the vec3 data into a shader.  
The shader moves the vertices corresponding to the row and column by the height value on the z axis.



## Controls

<strong>WASD : </strong> Move the camera  
<strong>MOUSE : </strong> Rotate the camera  
<strong>F : </strong> Toggle flat or smooth shading  
<strong>L : </strong>Toggle live or static data  
<strong> ARROW KEYS : </strong> Change the highlighted row or column  


## Build Instructions
Clone the repository  
Open the project solution (.sln) with Visual Studio    
Build the project (Ctrl + B)  
Run/Debug the project (F5) 
  
The project can be run in Debug or Release mode

## Standalone executable

The standalone executable can be found [here](https://github.com/ppouke/Morph/releases/tag/v1.0)  
MorphExecutable.zip file contains the executable as well as the  necessary shaders in a folder.

