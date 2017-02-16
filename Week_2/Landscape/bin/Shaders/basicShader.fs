#version 400											
in vec2 fUv;												
in vec3 fPos; 											
in vec4 fNormal;											
out vec4 frag_color;										
uniform sampler2D texture;								
uniform float lightAmbientStrength;						
uniform vec3 lightPosition;								
uniform vec3 lightColor;									
void main ()												
{															
  vec3 norm = normalize(fNormal.xyz);					
  vec3 lightDir = normalize(fPos - lightPosition);		
  float diff = max(dot(norm,lightDir),0.0f);				
  vec3 diffColor = diff * lightColor;				
  vec3 ambient = lightColor * lightAmbientStrength;		
  frag_color = texture2D(texture,fUv) * vec4(ambient + diffColor,1.0);
};
