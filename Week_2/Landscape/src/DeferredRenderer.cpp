#include "DeferredRenderer.h"
#include <iostream>


DeferredRenderer::DeferredRenderer()
{
	gBufferShader = new Shader("Lanscape/Shaders/Gbuffer");
	compShader = new Shader("Landscape/Shaders/compPass");
	directLight = new Shader("Landscape/Shaders/directLight");
}


DeferredRenderer::~DeferredRenderer()
{

}

void DeferredRenderer::CreateGPassBuffer(int windowWidth, int windowHeight)
{
	// setup gpass framebuffer
	glGenFramebuffers(1, &m_gpassFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gpassFBO);
	glGenTextures(1, &m_albedoTexture);
	glBindTexture(GL_TEXTURE_2D, m_albedoTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, windowWidth, windowHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenTextures(1, &m_positionTexture);
	glBindTexture(GL_TEXTURE_2D, m_positionTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, windowWidth, windowHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenTextures(1, &m_normalTexture);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, windowWidth, windowHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenRenderbuffers(1, &m_gpassDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gpassDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_albedoTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_positionTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_normalTexture, 0); 
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gpassDepth);
	GLenum gpassTargets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, gpassTargets);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::CreateLightBuffer(int windowWidth, int windowHeight)
{
	// setup light framebuffer
	glGenFramebuffers(1, &m_lightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);
	glGenTextures(1, &m_lightTexture);
	glBindTexture(GL_TEXTURE_2D, m_lightTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, windowWidth, windowHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_lightTexture, 0);
	GLenum lightTargets[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, lightTargets); 
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) 
		std::cout<< "Framebuffer Error!\n"; 
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::Render(Camera* m_camera)
{
	// 1st pass, G-Pass: render out the albedo, position and normal
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gpassFBO);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gBufferShader->m_program);
	// bind camera transforms, including view matrix separate
	int loc = glGetUniformLocation(gBufferShader->m_program, "projectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_camera->GetProjection()[0][0]));
	loc = glGetUniformLocation(gBufferShader->m_program,"view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_camera->GetView()[0][0]));
	// TODO: DRAW YOUR GEOMETRY HERE
	// in this example the Stanford Bunny
	// more passes to come...
}

void DeferredRenderer::drawDirectionalLight(const glm::vec3 & direction, const glm::vec3 & diffuse,Camera* m_camera)
{
	//glm::vec4 viewSpaceLight = m_camera->GetView() * glm::vec4(glm::normalize(direction), 0);
	//int loc = glGetUniformLocation(directLight->m_program, "lightDirection");
	//glUniform3fv(loc, 1, &viewSpaceLight[0]);
	//loc = glGetUniformLocation(directLight->m_program, "lightDiffuse");
	//glUniform3fv(loc, 1, &diffuse[0]);
	//glBindVertexArray(m_quadVAO);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
}
