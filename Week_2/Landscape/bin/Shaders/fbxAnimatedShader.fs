#version 410
in vec3 frag_normal;
in vec3 frag_position;
in vec3 frag_tangent;
in vec3 frag_bitangent;
in vec2 frag_texcoord;
out vec4 frag_color;
uniform sampler2D diffuseTexture;
void main()
{
    frag_color = texture2D(diffuseTexture,frag_texcoord);
}