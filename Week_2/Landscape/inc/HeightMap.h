#pragma once
#include <glm\glm.hpp>
#include <Texture.h>
#include <vector>
#include "Shader.h"
#include "Light.h"
#include "Camera.h"
namespace aie
{
	class Texture;
}
class HeightMap
{
public:
	HeightMap();
	~HeightMap();

	void CreateHeightMap();//Creates the heightmap
	void DrawHeightMap(glm::mat4 projectionView, std::vector<Light*> lightSources, Camera* camera);//Draws the heightmap
	void DestroyHeightMap();
	float timePassed;//Total time is used for the water shader
private:
	Shader* m_shader;
	std::vector<aie::Texture*> m_textures;
	enum TexturePositions
	{
		heightmap,
		grass,
		sand,
		snow,
		rock,
		splat,
		water,
		NUM_ITEMS
	};//Textures used for the heightmap
	TexturePositions m_texPositions;
	const char * m_textureNames[NUM_ITEMS] = {"grass","sand","snow","rock","splat","water"};
	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;

	unsigned int m_vertCount;
	unsigned int m_IndicesCount;

	int m_LandWidth = 512, m_LandDepth = 512;
	const float m_vertSeperation = 0.1f;
	const float m_maxHeight = 2;
	float waterAmplitude = 0.2f;
	float waterFrequency = 1.0f;
	float waterSpeed = 1.0f;
	struct Vertex
	{
		glm::vec4 pos;
		glm::vec2 uv;
		glm::vec4 vNormal;
		static void SetupVertexAttribPointers();
	};
};

