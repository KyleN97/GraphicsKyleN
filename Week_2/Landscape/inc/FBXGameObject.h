#pragma once
#include "FBXFile.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Light.h"
#include "Camera.h"
class GLMesh
{
public:
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
};
class FBXGameObject
{
public:
	FBXGameObject(const char* fileName,const char* shaderName, bool hasAnimations);
	~FBXGameObject();

	void CreateFBXOpenGLBuffers(FBXFile *fbx);
	
	void CreateFBXAnimatedOpenGLBuffers(FBXFile *fbx);

	void CleanupFBXOpenGLBuffers(FBXFile *file);

	void PlayAnimationTo(int a, int b);

	void Update(float d_time,float anim_Timer);

	void Draw(glm::mat4 projectionView,std::vector<Light*>lightSources,Camera* m_camera);

	void DrawUI(float d_time);

	void Translate(glm::vec3 transAmount) {
		modelTransforms.m_transform = modelTransforms.m_transform * glm::translate(transAmount);
	}
	void Scale(glm::vec3 scaleAmount) {
		modelTransforms.m_transform = modelTransforms.m_transform  * glm::scale(scaleAmount);
		//modelTransforms.m_scale = scaleAmount.x;
	}
	void Rotate(float angle,glm::vec3 rotAmount) {
		modelTransforms.m_transform = modelTransforms.m_transform *  glm::rotate(angle, rotAmount);
	}
	void SlerpTo(glm::vec3 a, glm::quat b) {
		modelTransforms.m_transform = glm::translate(a) * glm::toMat4(b);
	}
private:
	Shader* m_shader;//Shader for the model
	FBXFile *m_fbxFile;//File for the model
	FBXSkeleton* m_skeleton;//Skeletion for the model
	FBXAnimation* m_animation;//Animation for the model
	struct Transforms 
	{
		float m_scale = 1.0f;
		glm::quat rotation;
		glm::mat4 m_transform = {glm::vec4(m_scale,0,0,0),
								 glm::vec4(0,m_scale,0,0),
								 glm::vec4(0,0,m_scale,0),
								 glm::vec4(0,0,0,1)};
	};//Transforms data of the model
	Transforms modelTransforms;//Accesbile transforms of the model
	float fbxFrameCount;//amount of frames in animation
	float fbxCurrentFrame = 0;//frame display counter
	bool isAnimated = false;//is the fbx animated
};

