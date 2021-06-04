#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 light; // positional light direction

out vec3 fragNor;
out vec3 lightDir;
out vec3 EPos;

void main()
{
	gl_Position = P * V * M * vertPos;
	
	//update these as needed
	fragNor = (V * M * vec4(vertNor, 0.0)).xyz; 
	lightDir = (V * vec4(light, 0.0)).xyz;
	EPos = (V * M * vertPos).xyz;
}
