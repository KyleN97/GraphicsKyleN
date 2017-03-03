#pragma once
#include "Shader.h"
class PostProcessor
{
public:
	PostProcessor();
	~PostProcessor();
	void SetupFrameBuffer(unsigned int windowHeight,unsigned int windowWidth);//Setup the fram buffer
	void SetupFrameQuad(unsigned int windowHeight, unsigned int windowWidth);//Setup the frame quad
	void InitDrawPostProcess(bool isOn, unsigned int windowHeight, unsigned int windowWidth);//Initialise the Drawing process of the Fram Buffer
	void DrawPostProcess(bool isOn, unsigned int windowHeight, unsigned int windowWidth);//Draw the Frame Buffer
	void DrawPostProcessUI();//Draw the UI to change Post Processing modes
	bool m_enablePostProcess = false;

private:

	Shader* m_postProcessorShader;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_fbo;
	unsigned int m_fboTexture;
	unsigned int m_fboDepth;

	bool m_enableDistortion = true;
	bool m_enableBlur = false;
	bool m_enableGrey = false;
};

