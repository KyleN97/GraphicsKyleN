#version 430		
					
layout (location = 0) in vec4 vPosition;							
layout (location = 1) in vec2 vUv;							
layout (location = 2) in vec4 vNormal;						
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
