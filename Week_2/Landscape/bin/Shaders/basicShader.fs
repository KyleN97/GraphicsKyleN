#version 430	
										
in vec2 fUv;												
in vec3 fPos; 											
in vec4 fNormal;											
out vec4 frag_color;										
uniform sampler2D texture;
uniform sampler2D grass;	
uniform sampler2D rock;	
uniform sampler2D sand;	
uniform sampler2D snow;	
uniform float lightAmbientStrength;						
uniform vec3 lightPosition;								
uniform vec3 lightColor;	
uniform vec3 lightSpecColor;		
uniform float specPower = 32.0f;
uniform vec3 camPos;
					
void main ()												
{															
  vec3 norm = normalize(fNormal.xyz);					
  vec3 lightDir = normalize(lightPosition - fPos);		
  float diff = max(dot(norm,lightDir),0.0f);				
  vec3 diffColor = diff * lightColor;
	//ambient --START  
  vec3 ambient = lightColor * lightAmbientStrength;	
	//ambient --END  

	
	
  vec3 R = reflect(-lightDir,norm);
  vec3 E = normalize(camPos - fPos);
  float specTerm = pow(max(0.0f,dot(R,E)),specPower);
  vec3 Specular = lightSpecColor * specTerm;
  
  
  vec4 texColor;
  if(fPos.y > 1.0f)
	texColor = texture2D(snow,fUv);
  if(fPos.y > 0.5f && fPos.y < 1.0f)
	texColor = texture2D(grass,fUv);
  else
	texColor = texture2D(rock,fUv);

  // else
	// texColor = texture2D(texture,fUv);
  frag_color = texColor * vec4(ambient + diffColor + Specular,1.0);
  
  
 };
