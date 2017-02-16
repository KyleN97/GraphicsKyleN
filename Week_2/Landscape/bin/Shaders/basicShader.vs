
#version 400							
in vec4 vPosition;							
in vec2 vUv;							
in vec4 vNormal;						
out vec2 fUv;								
out vec3 fPos; 								
out vec4 fNormal; 							
uniform mat4 projectionView;				
void main ()								
{											
	fNormal = vNormal;						
	fPos = vPosition.xyz;						
	fUv = vUv;								
  gl_Position = projectionView * vPosition;	
};											
