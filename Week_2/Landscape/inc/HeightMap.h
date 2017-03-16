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
	void DrawHeightMap(glm::mat4 projectionView,Camera* camera, LightManager* gameLightManager);//Draws the heightmap
	void DestroyHeightMap();//Destroy the heightmap
	float timePassed;//Total time is used for the water shader
private:
	Shader* m_shader;//Shader for the heightmap
	std::vector<aie::Texture*> m_textures;//Textures for the heightmap
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
	TexturePositions m_texPositions;//Enum used to locate the texture positions

	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;
	//Vertex array + buffer objects, Index buffer objects

	unsigned int m_vertCount;
	unsigned int m_IndicesCount;
	//Vert and indices count of the heightmap

	int m_LandWidth = 512, m_LandDepth = 512;//Land Width and Depth (x & y) 
	const float m_vertSeperation = 0.1f;//Seperation between each vert
	const float m_maxHeight = 2;//Maximum height of the landscape
	float waterAmplitude = 0.2f;//Amplitude of the water
	float waterFrequency = 1.0f;//Frequence of the water
	float waterSpeed = 1.0f;//Speed of the water
	struct Vertex
	{
		glm::vec4 pos;
		glm::vec2 uv;
		glm::vec4 vNormal;
		static void SetupVertexAttribPointers();
	};//Vertex struct with pos, uv , normal and setup for attrub pointers
};

