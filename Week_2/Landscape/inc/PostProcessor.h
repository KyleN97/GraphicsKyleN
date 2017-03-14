#pragma once
#include "Shader.h"
class PostProcessor
{
public:
	PostProcessor();
	~PostProcessor();
	void SetupFrameBuffer(unsigned int windowWidth,unsigned int windowHeight);//Setup the fram buffer
	void SetupFrameQuad(unsigned int windowWidth, unsigned int windowHeight);//Setup the frame quad
	void InitDrawPostProcess(bool isOn, unsigned int windowWidth, unsigned int windowHeight);//Initialise the Drawing process of the Fram Buffer
	void DrawPostProcess(bool isOn, unsigned int windowWidth, unsigned int windowHeight);//Draw the Frame Buffer
	void DrawPostProcessUI();//Draw the UI to change Post Processing modes
	bool m_enablePostProcess = false;//Wether to enable or disable post processor

private:

	Shader* m_postProcessorShader;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_fbo;
	unsigned int m_fboTexture;
	unsigned int m_fboDepth;
	//Vertex array/buffer/frame object + Frame buffer Object Texture and Depth
	bool m_enableDistortion = true;
	bool m_enableBlur = false;
	bool m_enableGrey = false;
	//Post Processer modes
};

