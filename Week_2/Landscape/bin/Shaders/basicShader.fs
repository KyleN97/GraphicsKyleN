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
uniform float lightAmbientStrength;						
uniform vec3 lightPosition;								
uniform vec3 lightColor;	
uniform vec3 lightSpecColor;		
uniform float specPower = 32;
uniform vec3 camPos;
uniform float blend = 50.0f;
const float texSize = 10.0f;
uniform float Time;		
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
	texColor = sp.x * texture2D(rock,fUv* texSize);
	texColor += sp.y * texture2D(grass,fUv* texSize);
	if(sp.z >= 0.5f){
		texColor += mix(texture2D(water,fUv* texSize + Time / 24),texture2D(sand,fUv* texSize),(blend / 100)* -fPos.y);
	}		
	// if(sp.z >= 0.5f){
	// 	texColor += sp.z * texture2D(water,fUv* texSize + Time / 24);
		
	// }							
	frag_color = texColor * vec4(finalAmbient + totalDiffuse + totalSpecular,1.0f);
};
