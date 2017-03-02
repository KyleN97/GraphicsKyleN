#version 430										
in vec2 fUv;												
in vec3 fPos; 																	
out vec4 frag_color;										
uniform sampler2D texture;
					
void main ()												
{		
	frag_color = texture2D(texture,fUv);
};
