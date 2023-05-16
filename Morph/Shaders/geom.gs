#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


out vec3 Dist;
out vec3 Normal;
out vec3 vColor;
out vec3 FragPos;
out float Highlight;

in VS_OUT{
	vec3 vColor;
	float Highlight;
	vec3 Normal;
	vec3 vPos;
} gs_in[];


uniform bool shading = true;




vec3 GetNormal(int id)
{
	vec3 n = vec3(0);
	if(shading)
	{
		n = gs_in[id].Normal;	
	}
	else{

	vec3 a = gs_in[1].vPos - gs_in[0].vPos;
	vec3 b = gs_in[2].vPos - gs_in[0].vPos;
	
	n =  normalize(cross(a,b));
	}

	return n;
}

void main()
{
	gl_Position = gl_in[0].gl_Position;
	FragPos = vec3(gl_in[0].gl_Position);
	vColor = gs_in[0].vColor;
	Normal = GetNormal(0);
	Highlight = gs_in[0].Highlight;
	Dist = vec3(1,0,0);
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	FragPos = vec3(gl_in[1].gl_Position);
	vColor = gs_in[1].vColor;
	Normal = GetNormal(1);
	Highlight = gs_in[1].Highlight;

	Dist = vec3(0,1,0);
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	FragPos = vec3(gl_in[2].gl_Position);
	vColor = gs_in[2].vColor;
	Normal = GetNormal(2);
	Highlight = gs_in[2].Highlight;
	Dist = vec3(0,0,1);
	EmitVertex();
}

