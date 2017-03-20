#version 410
in vec4 vNormal;
in vec2 vuv; 
in vec3 fPos; 
out vec4 FragColor; 
uniform sampler2D diffuseTexture;
uniform float[3] lightAmbientStrength;						
uniform vec4 [3] lightPosition;								
uniform vec3 [3] lightColor;	
uniform vec3 [3] lightSpecColor;	
uniform float[3] attenuation;
uniform float[3] coneangle;
uniform vec3 [3] coneDirection;	
uniform float[3] specPower;
uniform int[3] lightType;
uniform vec3 camPos; 
void main()
{
	vec3 finalLighting = vec3(0,0,0);
	vec3 lightDir;
		for(int i = 0; i < 3; i++){
		float attenuationFactor = 1.0;
		if(lightType[i] == 0){
			lightDir = normalize(lightPosition[i].xyz);
			attenuationFactor = 1.0;
			//Directional Light
		}
		else if(lightType[i] == 1){
			//Point Light
			lightDir = normalize(lightPosition[i].xyz - fPos);
			attenuationFactor = 1.0;
		}
		else if (lightType[i] == 2){
			lightDir = normalize(lightPosition[i].xyz - fPos);
			float distanceToLight = length(lightPosition[i].xyz - fPos);
			lightDir = normalize(lightPosition[i].xyz - fPos);				 
			attenuationFactor = 1.0 / (1.0 + attenuation[i] * pow(distanceToLight,2));
    		float lightToSurfaceAngle = degrees(acos(dot(-lightDir,normalize(coneDirection[i]))));
			if(lightToSurfaceAngle > coneangle[i]){
			  	attenuationFactor = 0.0;
			}
			//Spot Light
		}

		
		vec3 norm 	= normalize(vNormal.xyz);		
		float diff 	= max(dot(norm, lightDir), 0.0);
		vec3 R		= reflect(-lightDir, norm);
		vec3 E	    = normalize(camPos - fPos); 
		float specTerm = pow(max(0.0, dot(R,E)), specPower[i]); 
		
		vec3 totalDiffuse  =  diff * lightColor[i]; 
		vec3 finalAmbient  =  lightColor[i] * lightAmbientStrength[i]; 
		vec3 totalSpecular =  lightSpecColor[i] * specTerm; 
		finalLighting +=  finalAmbient * attenuationFactor * (totalDiffuse + totalSpecular);
				
	}
	FragColor = texture2D(diffuseTexture, vuv) * vec4(finalLighting,1); 
}