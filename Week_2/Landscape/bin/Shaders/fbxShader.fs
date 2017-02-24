#version 410
in vec4 vNormal;
in vec2 vuv; 
in vec3 fPos; 
out vec4 FragColor; 
uniform sampler2D diffuseTexture;
uniform int lightSourceCount = 0; 
uniform float lightAmbientStrength;
uniform vec3 lightPosition[64]; 
uniform vec3 lightColor[64]; 
//uniform vec3 lightSpecColor[64]; 
uniform float specPower = 32.0f;

//uniform vec4 modelSpecular;
//uniform vec4 modelAmbient;
//uniform vec4 modelDiffuse;
uniform vec3 camPos; 
void main() { 
	for(int i = 0;i < lightSourceCount + 1; i++){
		vec3 norm = normalize(vNormal.xyz);
		vec3 lightDir = normalize(lightPosition[i] - fPos);
		float diff = max(dot(norm, lightDir), 0.0f);
		vec3 diffColor = diff * lightColor[i]; 
		vec3 ambient = lightColor[i] * lightAmbientStrength; 
		vec3 R = reflect(-lightDir, norm);
		vec3 E = normalize(camPos - fPos); 
		float specTerm = pow(max(0.0f, dot(R, E)), specPower); 
		vec3 Specular = lightColor[i] * specTerm; 
		FragColor = texture2D(diffuseTexture, vuv) * vec4(Specular + ambient + diffColor,1); 
	}
};