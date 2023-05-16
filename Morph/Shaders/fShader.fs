#version 330 core

struct Material{
	sampler2D texture_specular1;
	sampler2D texture_diffuse1;
	float shininess;

};

struct PointLight {
	vec3 position;
	//float cutOff;
	//float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	

	float constant;
	float linear;
	float quadratic;
};

struct DirLight{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 vColor;
in vec3 Dist;
in float Highlight;

uniform Material material;


#define NR_POINT_LIGHTS 3  
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light,vec3 normal, vec3 viewDir, vec3 color){
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir),0), material.shininess);

	


	vec3 ambient = light.ambient * color;
	vec3 diffuse = light.diffuse * (diff * color);
	vec3 specular = light.specular * spec; 



	return ambient + specular + diffuse;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color){
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float diff = max(dot(normal, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
	
	
	
	float distance = length(light.position - fragPos);
	float attenuation = 1/(light.constant + light.linear * distance + light.quadratic * (distance * distance));

	
	vec3 ambient = light.ambient * color;
	vec3 diffuse = light.diffuse * (diff * color);
	vec3 specular = light.specular * spec; 

	

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;


	return ambient + diffuse + specular;
}




void main()
{
	//calculate wireframe using barycentric coordinates
	float minBary  = min(Dist.x, min(Dist.y,Dist.z));
	float delta = fwidth(minBary);
	minBary = smoothstep(0,delta, minBary);

	// add wireframe color to vertex color
	vec3 color = mix(vec3(0.1), vColor, minBary);

	
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	//direction light
	vec3 result = CalcDirLight(dirLight, norm, viewDir, color);

	//pointlights
	for(int i = 0; i < NR_POINT_LIGHTS; i++){
		//result += CalcPointLight(pointLights[i], norm, FragPos,  viewDir, color);
	}

	 
	
	//add Highlight
	float clampedHighlight = smoothstep(0.95,1, Highlight);
	float edgeHiglight = (1-minBary)*clampedHighlight;
	result += vec3(edgeHiglight * vec3(0,1,0));


	FragColor = vec4(result, 1.0f);
}