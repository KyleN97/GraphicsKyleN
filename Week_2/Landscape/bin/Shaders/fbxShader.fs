#version 410
in vec4 vNormal;
in vec2 vuv; 
in vec3 fPos; 
out vec4 FragColor; 
uniform sampler2D diffuseTexture;
uniform float lightAmbientStrength;
uniform vec3 lightPosition; 
uniform vec3 lightColor; 
uniform vec3 lightSpecColor; 
uniform float specPower = 32.0f;
uniform vec3 camPos; 
void main() { 

		vec3 norm = normalize(vNormal.xyz);
		vec3 lightDir = normalize(lightPosition - fPos);
		float diff = max(dot(norm, lightDir), 0.0f);
		vec3 diffColor = diff * lightColor; 
		vec3 ambient = lightColor * lightAmbientStrength; 
		vec3 R = reflect(-lightDir, norm);
		vec3 E = normalize(camPos - fPos); 
		float specTerm = pow(max(0.0f, dot(R, E)), specPower); 
		vec3 Specular = lightColor * specTerm; 
		FragColor = texture2D(diffuseTexture, vuv) * vec4(Specular + ambient + diffColor,1); 

};