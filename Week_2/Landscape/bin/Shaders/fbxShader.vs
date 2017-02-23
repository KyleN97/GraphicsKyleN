#version 410
in vec4 position; 
in vec4 normal; 
in vec2 uv;
out vec4 vNormal;
out vec2 vuv; 
out vec3 fPos; 
uniform mat4 projectionViewWorldMatrix; 
void main() 
{ 
	vNormal = normal; 
	vuv = uv; 
	fPos = position.xyz;
	gl_Position = projectionViewWorldMatrix * position;
};