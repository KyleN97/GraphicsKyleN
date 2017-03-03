#version 430										
in vec2 fUv;												
in vec3 fPos; 											
in vec4 fNormal;											
out vec4 frag_color;										
uniform sampler2D grass;	
uniform sampler2D rock;	
uniform sampler2D sand;	
uniform sampler2D snow;
uniform sampler2D splat;	
uniform float lightAmbientStrength;						
uniform vec3 lightPosition;								
uniform vec3 lightColor;	
uniform vec3 lightSpecColor;		
uniform float specPower = 32;
uniform vec3 camPos;
					
void main ()												
{		
		
	vec3 norm = normalize(fNormal.xyz);

	vec3 lightDir = normalize(lightPosition - fPos);
	float diff = max(dot(norm, lightDir), 0.0);

	vec3 R = reflect(-lightDir, norm);
	vec3 E = normalize(camPos - fPos); 
	float specTerm = pow(max(0.0, dot(R, E)), specPower); 
		
	vec3 totalDiffuse = diff * lightColor; 
	vec3 finalAmbient =  lightColor * lightAmbientStrength; 
	vec3 totalSpecular =  lightSpecColor * specTerm; 

	  		
	vec4 texColor;
	vec4 sp = texture2D(splat,fUv);
	texColor = sp.x * texture2D(snow,fUv);
		texColor += sp.y * texture2D(grass,fUv);
			texColor += sp.z * texture2D(rock,fUv);
	frag_color = texColor * vec4(finalAmbient + totalDiffuse + totalSpecular,1.0);
};
