#version 330 core
uniform sampler2D Texture0;
uniform int flip;

in vec2 vTexCoord;
in vec3 fragNor; //interpolated normal and light vector in camera space
in vec3 lightDir;
in vec3 EPos; //position of the vertex in camera space

out vec4 Outcolor;

void main() {
	vec4 texColor0 = texture(Texture0, vTexCoord);
	if (texColor0.a < 0.1)
        discard;

	vec4 matDif = 0.6*texColor0;
	vec4 matSpec = 0.3*texColor0;
	vec4 matAmb = 0.2*texColor0;
	float matShine = 5;

	vec3 normal = normalize(fragNor);
	if (flip < 1)
		normal *= -1.0f;
	vec3 light = normalize(lightDir);
	vec3 halfway = normalize(normalize(-1*EPos) + light);

	Outcolor = vec4(matAmb + matDif*max(0, dot(normal, light)) + matSpec*pow(max(0, dot(normal, halfway)), matShine));
  
  	// To set the outcolor as the texture coordinate (for debugging)
	// Outcolor = vec4(vTexCoord.s, vTexCoord.t, 0, 1);
}