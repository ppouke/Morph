#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (std140, binding = 0) uniform Matrices {
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

uniform vec3 vertexColor;
uniform vec2 highlights;



#define NR_VERTS_PER_SIDE 20

uniform vec3 data[NR_VERTS_PER_SIDE*NR_VERTS_PER_SIDE];



out VS_OUT{
    vec3 vColor;
    float Highlight;
    vec3 Normal;
    vec3 vPos;

} vs_out;



float HeightfromIndex(vec2 id2, int sideLength)
{
    float height = 0;
    for (int i = 0; i < sideLength * sideLength; i++)
    {
        if(id2 == vec2(data[i].x, data[i].y)) height = data[i].z;
    }

    return height;
}

vec3 CalculateNormal(vec3 v0, vec2 id2, int sideLength)
{   
    
    vec3 v1 = v0 + vec3(0.05,0,0);
    vec3 v2 = v0 + vec3(0,0.05,0);
    
    
    float h1 = 0;
    if(id2.x < sideLength){
        h1 = HeightfromIndex(vec2(id2.x+1,id2.y), sideLength);
    }
    else {
        h1 = HeightfromIndex(vec2(id2.x-1,id2.y), sideLength); 
        //v2.x = 0.1;
    }
    v1.z = h1;

    float h2 = 0;
    if(id2.y < sideLength)
    {
        h2 = HeightfromIndex(vec2(id2.x, id2.y +1), sideLength);
    }
    else
    {
        h2 = HeightfromIndex(vec2(id2.x, id2.y +1), sideLength);
        //v2.z = -0.1;
    }

    v2.z = h2;
    
    return normalize(cross(v1-v0,v2-v0));
    

}

float AddHighlight(vec2 id2)
{
    
    float h = id2.x - highlights.x;
    float x = step(abs(h),0.5);

    h = id2.y - highlights.y;
    float y = step(abs(h), 0.5);
    

    return step(0.5, x+y);
    
}

void main()
{


    vec3 nPos = aPos;
    vec2 id2 = vec2(gl_VertexID % NR_VERTS_PER_SIDE, gl_VertexID / NR_VERTS_PER_SIDE);

    float h = HeightfromIndex(id2, NR_VERTS_PER_SIDE);
    nPos.z += h;
    gl_Position = projection * view * model * vec4(nPos,1.0f);
    
    //Normal = mat3(transpose(inverse(model))) * aNormal; //Note: costly to calc inverse
    vec3 n  = CalculateNormal(nPos, id2, NR_VERTS_PER_SIDE);
    
   
    n = mat3(transpose(inverse(model))) * n;


    vs_out.Normal = n;
    vs_out.Highlight = AddHighlight(id2);

    
    vs_out.vColor = vertexColor;
    vs_out.vPos = vec4(model * vec4(nPos, 1.0f)).xyz;
    //FragPos = vec3(model * vec4(nPos, 1.0f));
}

