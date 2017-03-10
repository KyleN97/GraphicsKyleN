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
uniform float[2] lightAmbientStrength;						
uniform vec4[2] lightPosition;								
uniform vec3[2] lightColor;	
uniform vec3[2] lightSpecColor;	
uniform float[2] attenuation;
uniform float[2] coneangle;
uniform vec3[2] coneDirection;	
uniform float specPower = 32;
uniform vec3 camPos;
uniform float blend = 50.0f;
const float texSize = 10.0f;
uniform float Time;	
vec3 finalLighting = vec3(0,0,0);	
void main ()												
{		
	vec3 lightDir;
	vec4 texColor;
	for(int i = 0; i < 2; i++){
		float attenuationFactor = 1.0;
		if(lightPosition[i].w  == 0){
			lightDir = normalize(lightPosition[i].xyz);
			attenuationFactor = 1.0;
		}
		else{
			////spotlight
			 float distanceToLight = length(lightPosition[i].xyz - fPos);
			 lightDir = normalize(lightPosition[i].xyz - fPos);				 
			 attenuationFactor = 1.0 / (1.0 + attenuation[i] * pow(distanceToLight,2));
 
			 //cone
			  float lightToSurfaceAngle = degrees(acos(dot(-lightDir,normalize(coneDirection[i]))));
			  if(lightToSurfaceAngle > coneangle[i]){
			  	attenuationFactor = 0.0;
			  }

		}
		
		vec3 norm 	= normalize(fNormal.xyz);		
		float diff 	= max(dot(norm, lightDir), 0.0);
		vec3 R		= reflect(-lightDir, norm);
		vec3 E	    = normalize(camPos - fPos); 
		float specTerm = pow(max(0.0, dot(R,E)), specPower); 
		
		vec3 totalDiffuse  =  diff * lightColor[i]; 
		vec3 finalAmbient  =  lightColor[i] * lightAmbientStrength[i]; 
		vec3 totalSpecular =  lightSpecColor[i] * specTerm; 
		finalLighting +=  finalAmbient + attenuationFactor * (totalDiffuse + totalSpecular);
				
	}
	vec4 sp = texture2D(splat,fUv);

	texColor = sp.x * texture2D(rock,fUv * texSize);
	texColor += sp.y * texture2D(grass,fUv* texSize);

	if(sp.z >= 0.5f){
		texColor += mix(texture2D(water,fUv * texSize + Time / 24),texture2D(sand,fUv* texSize),(blend / 100)* -fPos.y);
	}	

	frag_color = texColor * vec4(finalLighting,1.0);	
	
};
