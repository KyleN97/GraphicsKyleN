#include "PostProcessor.h"
#include "gl_core_4_4.h"
#include <glm/vec2.hpp>
#include "imgui.h"
PostProcessor::PostProcessor()
{
	m_postProcessorShader = new Shader("./Landscape/Shaders/frameBufferShader");
	//Setup the post processor shader
}

PostProcessor::~PostProcessor()
{
	delete m_postProcessorShader;//destroy shader
}

void PostProcessor::SetupFrameBuffer(unsigned int windowWidth, unsigned int windowHeight)
{
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glGenTextures(1, &m_fboTexture);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, windowWidth, windowHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0);
	glGenRenderbuffers(1, &m_fboDepth); glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Generate all buffers ,setup textrues and draw and bind all of those buffers + textures
}

void PostProcessor::SetupFrameQuad(unsigned int windowWidth, unsigned int windowHeight)
{
	glm::vec2 texelSize = 1.0f / glm::vec2(windowWidth, windowHeight);
	// full screen quad
	glm::vec2 halfTexel = 1.0f / glm::vec2(windowWidth, windowHeight) * 0.5f;
	float vertexData[] = { -1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y, -1, 1, 0, 1,
		halfTexel.x, 1 - halfTexel.y, -1, -1, 0, 1, halfTexel.x,
		halfTexel.y, 1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y, };
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, vertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Generate the vertex array objects and setup all the pointers + attrib arrays
}

void PostProcessor::InitDrawPostProcess(bool isOn, unsigned int windowWidth, unsigned int windowHeight)
{
	if (isOn)
	{
		// bind our target
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glViewport(0, 0, windowWidth, windowHeight);
		// clear the target 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void PostProcessor::DrawPostProcess(bool isOn, unsigned int windowWidth, unsigned int windowHeight)
{
	if (isOn)//If we are Post Processing
	{

		// bind the back-buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, windowWidth, windowHeight);
		// just clear the back-buffer depth as 
		//each pixel will be filled
		glClear(GL_DEPTH_BUFFER_BIT);
		// draw our full-screen quad
		glUseProgram(m_postProcessorShader->m_program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_fboTexture);
		int loc2 = glGetUniformLocation(m_postProcessorShader->m_program, "target");
		glUniform1i(glGetUniformLocation(m_postProcessorShader->m_program, "distort"), m_enableDistortion);
		glUniform1i(glGetUniformLocation(m_postProcessorShader->m_program, "blur"), m_enableBlur);
		glUniform1i(glGetUniformLocation(m_postProcessorShader->m_program, "greyScale"), m_enableGrey);
		//Pass in the Post Process texture and values to determine what Post Processing mode we are in
		glUniform1i(loc2, 0);
		glBindVertexArray(m_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//Draw the Frame
		glUseProgram(0);
	}
}

void PostProcessor::DrawPostProcessUI()
{
	ImGui::Begin("Post Processing");
	ImGui::Checkbox("Enable Post Processing", &m_enablePostProcess);
	ImGui::End();
	if (m_enablePostProcess)
	{
		ImGui::Begin("Post Processing Settings");
		ImGui::Checkbox("Distortion", &m_enableDistortion);
		ImGui::Checkbox("Blur", &m_enableBlur);
		ImGui::Checkbox("Grey Scale", &m_enableGrey);
		if (m_enableDistortion == false && m_enableBlur == false &&
			m_enableGrey == false)
		{
			m_enablePostProcess = false;
			m_enableDistortion = true;
		}
		ImGui::End();
	}
	//Draw the Post Process Ui to change modes and enable/disable
}
