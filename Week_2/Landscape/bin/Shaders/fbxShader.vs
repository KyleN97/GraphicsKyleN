#version 410
layout (location = 0) in vec4 position; 
layout (location = 1) in vec4 normal; 
layout (location = 2) in vec2 uv;
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