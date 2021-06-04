#version 330 core 
out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;

//interpolated normal and light vector in camera space
in vec3 fragNor;
in vec3 lightDir;
in vec3 EPos; //position of the vertex in camera space

void main()
{
	//you will need to work with these for lighting
	vec3 normal = normalize(fragNor);
	vec3 light = normalize(lightDir);
	vec3 halfway = normalize(normalize(-1*EPos) + light);

	color = vec4(MatAmb + MatDif*max(0, dot(normal, light)) + MatSpec*pow(max(0, dot(normal, halfway)), MatShine), 1.0);
}
