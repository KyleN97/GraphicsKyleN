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
uniform sampler2D water;	
uniform float[32] lightAmbientStrength;						
uniform vec4[32] lightPosition;								
uniform vec3[32] lightColor;	
uniform vec3[32] lightSpecColor;	
uniform float[32] attenuation;
uniform float[32] coneangle;
uniform vec3[32] coneDirection;	
uniform float specPower = 32;
uniform vec3 camPos;
uniform float blend = 50.0f;
const float texSize = 10.0f;
uniform float Time;		
void main ()												
{		
	vec3 norm[32];
	vec3 lightDir[32];
	float diff[32];
	vec3 R[32];
	vec3 E[32];
	float specTerm[32];
	vec3 totalDiffuse[32]; 
	vec3 finalAmbient[32]; 
	vec3 totalSpecular[32];
	vec4 texColor;
	vec3 finalLighting;
	


	for(int i = 0; i < 2; ++i){
		float attenuationFactor = 1.0;
		if(lightPosition[i].w  == 0){
			lightDir[i] = normalize(lightPosition[i].xyz);
			attenuationFactor = 1.0;
		}
		else{
			 lightDir[i] = normalize(lightPosition[i].xyz - fPos);	
			// //spotlight
			 float distanceToLight = length(lightPosition[i].xyz - fPos);
			 attenuationFactor = 1.0 / (1.0 + attenuation[i] * pow(distanceToLight,2));
 
			 //cone
			 float lightToSurfaceAngle = degrees(acos(dot(-lightDir[i],normalize(coneDirection[i]))));
			  if(lightToSurfaceAngle > coneangle[i]){
			  	attenuationFactor = 0.0;
			  }

		}
		norm[i] 	= normalize(fNormal.xyz);		
		diff[i] 	= max(dot(norm[i], lightDir[i]), 0.0);
		R[i] 		= reflect(-lightDir[i], norm[i]);
		E[i]	    = normalize(camPos - fPos); 
		specTerm[i] = pow(max(0.0, dot(R[i], E[i])), specPower); 
		
		totalDiffuse[i]  =  diff[i] * lightColor[i]; 
		finalAmbient[i]  =  lightColor[i] * lightAmbientStrength[i]; 
		totalSpecular[i] =  lightSpecColor[i] * specTerm[i]; 
		finalLighting +=  finalAmbient[i] + attenuationFactor * (totalDiffuse[i] + totalSpecular[i]);
				
	}
	vec4 sp = texture2D(splat,fUv);

	texColor = sp.x * texture2D(rock,fUv * texSize);
	texColor += sp.y * texture2D(grass,fUv* texSize);

	if(sp.z >= 0.5f){
		texColor += mix(texture2D(water,fUv* texSize + Time / 24),texture2D(sand,fUv* texSize),(blend / 100)* -fPos.y);
	}	

	frag_color = texColor * vec4(finalLighting,1.0);	
	
};
