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
uniform sampler2D splat;	
uniform int lightSourceCount; 
uniform float lightAmbientStrength;						
uniform vec3 lightPosition[64];								
uniform vec3 lightColor[64];	
uniform vec3 attenuation[64];
//uniform vec3 lightSpecColor;		
uniform float specPower = 32.0f;
vec3 toLightVector[64];
uniform vec3 camPos;
					
void main ()												
{				
	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);
	vec3 finalAmbient = vec3(0.0);
	for(int i = 0;i < lightSourceCount + 1; ++i){	
	
		float distance = length(lightPosition[i] - fPos);
		vec3 norm = normalize(fNormal.xyz);
		float attFactor = attenuation[i].x + (attenuation[i].y * distance) + (attenuation[i].z * distance * distance);
		toLightVector[i] = lightPosition[i] - fPos;
		
		vec3 lightDir = normalize(toLightVector[i]);
		float diff = max(dot(norm, lightDir), 0.0f);

		vec3 R = reflect(-lightDir, norm);
		vec3 E = normalize(camPos - fPos); 
		float specTerm = pow(max(0.0f, dot(R, E)), specPower); 
		
		totalDiffuse = totalDiffuse + (diff * lightColor[i]) / attFactor; 
		finalAmbient = lightAmbientStrength * lightColor[i]; 
		totalSpecular = totalSpecular + (specTerm * lightColor[i]) / attFactor; 
		
		if(i == 1){
			totalDiffuse = vec3(1,0,1);
			totalSpecular = vec3(1,0,1);
			finalAmbient = vec3(1,0,1);
		}
	}
	  		
		vec4 texColor;
		vec4 sp = texture2D(splat,fUv);
		texColor = sp.x * texture2D(snow,fUv);
		texColor += sp.y * texture2D(grass,fUv);
		texColor += sp.z * texture2D(rock,fUv);
		frag_color = texColor * vec4(finalAmbient + totalDiffuse + totalSpecular,1.0);
 };
