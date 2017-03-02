#version 430		
					
layout (location = 0) in vec4 vPosition;							
layout (location = 1) in vec2 vUv;							
out vec2 fUv;								
out vec3 fPos; 								
 							
uniform mat4 projectionView;				
void main ()								
{											
				
	fPos = vPosition.xyz;						
	fUv = vUv;								
  gl_Position = projectionView * vPosition;	
};											
