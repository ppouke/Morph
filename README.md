# Morph

Morph a mesh based on vec3 data in the form (row, column, value).  
The application procedurally creates a 20x20 grid and reads the vec3 data into a shader.  
The shader moves the vertices corresponding to the row and column by the height value on the z axis.


## Implementation in a nutshell

The application draws a 20x20 grid and assings the following vertex, geometry and fragment shader to it. A data array and other float and bool variables are fed to the shaders. 

### Vertex Shader
The vertex shader caclulates the new z values for each vertex based on a given uniform vec3 array.  
The vertex shader also calculates smooth normals for eaxch vertex by sampling an adjacent row and column to create a tangent and bitangent vector.  
The cross product of these vectors gives us the new normal for the vertex and this is passed on to the geometry shader.
The shader also forwards the current highlighted row and column, based on the vertexID and a given row and column index.

### Geometry Shader
The geometry shader handles creating flat shading by cross multiplying two of the vectors from the triangles edges for a normal vector and applying it to each vertex.  
A uniform bool value is read in this shader to decide the type of shading (flat/smooth) wanted.   
The geometry shader also passes barycentric coordinates to the fragment shader for creating a solid wireframe on the mesh.  

### Fragment Shader
The fragment shader handles colorizing and lighting the mesh. The current implmentation only calculates a directional light, though point lights are possible aswell.  
THe fragment shader also handles creating the wireframe on the mesh and colourising the highlighted row and column.




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

The standalone executable (x64) can be found [here](https://github.com/ppouke/Morph/releases/tag/v1.0)  
MorphExecutable.zip file contains the executable as well as the  necessary shaders in a folder.

