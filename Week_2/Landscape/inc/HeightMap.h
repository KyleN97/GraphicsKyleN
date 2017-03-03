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

	void CreateHeightMap();
	void DrawHeightMap(glm::mat4 projectionView, std::vector<Light*> lightSources, Camera* camera);
	void DestroyHeightMap();

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
		NUM_ITEMS
	};//Textures used for the heightmap
	TexturePositions m_texPositions;

	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;

	unsigned int m_vertCount;
	unsigned int m_IndicesCount;

	int m_LandWidth = 512, m_LandDepth = 512;
	const float m_vertSeperation = 0.1f;
	const float m_maxHeight = 2;

	struct Vertex
	{
		glm::vec4 pos;
		glm::vec2 uv;
		glm::vec4 vNormal;
		static void SetupVertexAttribPointers();
	};
};

