#version 410
in vec3 frag_normal;
in vec3 frag_position;
in vec3 frag_tangent;
in vec3 frag_bitangent;
in vec2 frag_texcoord;
out vec4 frag_color;
uniform sampler2D diffuseTexture;
uniform float lightAmbientStrength;
uniform vec3 lightPosition; 
uniform vec3 lightColor; 
uniform vec3 lightSpecColor; 
uniform float specPower = 32.0f;
uniform vec3 camPos; 
void main()
{
    vec3 norm = normalize(frag_normal);
	vec3 lightDir = normalize(lightPosition - frag_position);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffColor = diff * lightColor; 
	vec3 ambient = lightColor * lightAmbientStrength; 
	vec3 R = reflect(-lightDir, norm);
	vec3 E = normalize(camPos - frag_position); 
	float specTerm = pow(max(0.0f, dot(R, E)), specPower); 
	vec3 Specular = lightColor * specTerm; 
	frag_color = texture2D(diffuseTexture, frag_texcoord) * vec4(Specular + ambient + diffColor,1); 

}