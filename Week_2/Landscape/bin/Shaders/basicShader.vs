#version 430		
					
layout (location = 0) in vec4 vPosition;							
layout (location = 1) in vec2 vUv;							
layout (location = 2) in vec4 vNormal;						
out vec2 fUv;								
out vec3 fPos; 								
out vec4 fNormal; 							
uniform mat4 projectionView;	
uniform sampler2D splat;	
uniform float Time;		
uniform float amplitude = 1;
uniform float frequency =1 ;
uniform float speed = 1;

void main ()								
{											
	fNormal = vNormal;
	fUv = vUv;			
		
	fPos = vPosition.xyz;	
						
	vec4 sp = texture2D(splat,fUv);
			if(sp.z >= 0.5f){
				fPos.y += sin((frequency * fPos.x)+(Time * frequency))*amplitude;
		}							
  gl_Position = projectionView * vec4(fPos,1);	
};											
