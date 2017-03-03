#include "FBXGameObject.h"
#include "gl_core_4_4.h"
#include "imgui.h"

FBXGameObject::FBXGameObject(const char* fileName,const char* shaderName,bool hasAnimations)
{
	m_fbxFile = new FBXFile();
	m_skeleton = new FBXSkeleton();
	m_animation = new FBXAnimation();

	m_fbxFile->load(fileName,FBXFile::UNITS_CENTIMETER);
	if (hasAnimations) {
		CreateFBXAnimatedOpenGLBuffers(m_fbxFile);
		fbxFrameCount = m_fbxFile->getAnimationByIndex(0)->totalFrames();
		fbxCurrentFrame = 0.0f;
	}
	else {
		CreateFBXOpenGLBuffers(m_fbxFile);
	}
	isAnimated = hasAnimations;
	m_shader = new Shader(shaderName);
	//Load in the fbx and create a shader and determine wether it is animated or nor and setup buffers dependent
}


FBXGameObject::~FBXGameObject()
{
	CleanupFBXOpenGLBuffers(m_fbxFile);
	m_fbxFile->unload();
}

void FBXGameObject::CreateFBXOpenGLBuffers(FBXFile * fbx)
{
	// FBX Files contain multiple meshes, each with seperate material information
	// loop through each mesh within the FBX file and cretae VAO, VBO and IBO buffers for each mesh.
	// We can store that information within the mesh object via its "user data" void pointer variable.
	for (unsigned int i = 0; i < fbx->getMeshCount(); i++)
	{
		// get the current mesh from file
		FBXMeshNode *fbxMesh = fbx->getMeshByIndex(i);
		GLMesh *glData = new GLMesh();
		glGenVertexArrays(1, &glData->vao);
		glBindVertexArray(glData->vao);
		glGenBuffers(1, &glData->vbo);
		glGenBuffers(1, &glData->ibo);
		glBindBuffer(GL_ARRAY_BUFFER, glData->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData->ibo);
		// fill the vbo with our vertices.
		// the FBXLoader has convinently already defined a Vertex Structure for us.
		glBufferData(GL_ARRAY_BUFFER,
			fbxMesh->m_vertices.size() * sizeof(FBXVertex),
			fbxMesh->m_vertices.data(), GL_STATIC_DRAW);
		// fill the ibo with the indices.
		// fbx meshes can be large, so indices are stored as an unsigned int.
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			fbxMesh->m_indices.size() * sizeof(unsigned int),
			fbxMesh->m_indices.data(), GL_STATIC_DRAW);
		// Setup Vertex Attrib pointers
		// remember, we only need to setup the approprate attributes for the shaders that will be rendering
		// this fbx object.
		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);
		glEnableVertexAttribArray(2); // uv
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
		// TODO: add any additional attribute pointers required for shader use.
		// unbind
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		// attach our GLMesh object to the m_userData pointer.
		fbxMesh->m_userData = glData;
	}
}

void FBXGameObject::CreateFBXAnimatedOpenGLBuffers(FBXFile * fbx)
{
	// FBX Files contain multiple meshes, each with seperate material information
	// loop through each mesh within the FBX file and cretae VAO, VBO and IBO buffers for each mesh.
	// We can store that information within the mesh object via its "user data" void pointer variable.
	for (unsigned int i = 0; i < fbx->getMeshCount(); i++)
	{
		// get the current mesh from file
		FBXMeshNode *fbxMesh = fbx->getMeshByIndex(i);
		GLMesh *glData = new GLMesh();
		glGenVertexArrays(1, &glData->vao);
		glBindVertexArray(glData->vao);
		glGenBuffers(1, &glData->vbo);
		glGenBuffers(1, &glData->ibo);
		glBindBuffer(GL_ARRAY_BUFFER, glData->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData->ibo);
		// fill the vbo with our vertices.
		// the FBXLoader has convinently already defined a Vertex Structure for us.
		glBufferData(GL_ARRAY_BUFFER,
			fbxMesh->m_vertices.size() * sizeof(FBXVertex),
			fbxMesh->m_vertices.data(), GL_STATIC_DRAW);
		// fill the ibo with the indices.
		// fbx meshes can be large, so indices are stored as an unsigned int.
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			fbxMesh->m_indices.size() * sizeof(unsigned int),
			fbxMesh->m_indices.data(), GL_STATIC_DRAW);
		// Setup Vertex Attrib pointers
		// remember, we only need to setup the approprate attributes for the shaders that will be rendering
		// this fbx object.
		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normals
		glEnableVertexAttribArray(2); //tangents
		glEnableVertexAttribArray(3); //texcoords
		glEnableVertexAttribArray(4); //weights
		glEnableVertexAttribArray(5); //indices
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
			(void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),
			(void*)FBXVertex::NormalOffset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),
			(void*)FBXVertex::TangentOffset);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
			(void*)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
			(void*)FBXVertex::WeightsOffset);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
			(void*)FBXVertex::IndicesOffset);
		// TODO: add any additional attribute pointers required for shader use.
		// unbind
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		// attach our GLMesh object to the m_userData pointer.
		fbxMesh->m_userData = glData;
	}
}

void FBXGameObject::CleanupFBXOpenGLBuffers(FBXFile * file)
{
	for (unsigned int i = 0; i < file->getMeshCount(); i++)
	{
		FBXMeshNode *fbxMesh = file->getMeshByIndex(i);
		GLMesh *glData = (GLMesh *)fbxMesh->m_userData;
		glDeleteVertexArrays(1, &glData->vao);
		glDeleteBuffers(1, &glData->vbo);
		glDeleteBuffers(1, &glData->ibo);
		delete glData;
	}
}

void FBXGameObject::PlayAnimationTo(int a, int b)
{
	m_animation->m_startFrame = a;
	m_animation->m_endFrame = b;
	fbxCurrentFrame = m_animation->m_startFrame;
	//Will change the start and end frames of the animation and play the anim for that duration
}

void FBXGameObject::Update(float d_time, float anim_Timer)
{
	if (isAnimated)
	{
		m_skeleton = m_fbxFile->getSkeletonByIndex(0);
		m_animation = m_fbxFile->getAnimationByIndex(0);
		m_skeleton->evaluate(m_animation, anim_Timer);
		for (unsigned int bone_index = 0; bone_index < m_skeleton->m_boneCount; ++bone_index)
		{
			m_skeleton->m_nodes[bone_index]->updateGlobalTransform();
		}
		//Update the bones - skeleton and animation if it is animated
	}
}

void FBXGameObject::Draw(glm::mat4 projectionView, std::vector<Light*>lightSources, Camera* m_camera)
{
	if (isAnimated){
		//------ANIMTED FBX-----
		//bind the shader
		m_shader->Bind();
		// grab the skeleton and animation we want to use
		m_skeleton->updateBones();

		glUniformMatrix4fv(glGetUniformLocation(m_shader->m_program, "projectionViewWorldMatrix"),
			1, GL_FALSE, glm::value_ptr(projectionView * modelTransforms.m_transform));
		//pass in the proj matrix to the shader
		for (unsigned int i = 0; i < m_fbxFile->getMeshCount(); ++i)
		{

			FBXMeshNode* mesh = m_fbxFile->getMeshByIndex(i);
			GLMesh* glData = (GLMesh*)mesh->m_userData;
			// get the texture from the model
			unsigned int diffuseTexture = mesh->m_material->textureIDs[0];
			int bones_location = glGetUniformLocation(m_shader->m_program, "bones");
			// bid the texture and send it to our shader
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseTexture);
			glUniform1i(glGetUniformLocation(m_shader->m_program, "diffuseTexture"), 0);
			glUniformMatrix4fv(bones_location, m_skeleton->m_boneCount, GL_FALSE, (float*)m_skeleton->m_bones);
			//pass in the diffuse texture and bones
			// draw the mesh
			glBindVertexArray(glData->vao);
			glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		glUseProgram(0);
	}
	else {//If not animated
		//FBX - START
		m_shader->Bind();

		// send uniform variables, in this case the "projectionViewWorldMatrix"
		unsigned int mvpLoc = glGetUniformLocation(m_shader->m_program, "projectionViewWorldMatrix");
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(projectionView *
		modelTransforms.m_transform));
		// loop through each mesh within the fbx file
		for (unsigned int i = 0; i < m_fbxFile->getMeshCount(); ++i)
		{
			FBXMeshNode* mesh = m_fbxFile->getMeshByIndex(i);
			GLMesh* glData = (GLMesh*)mesh->m_userData;
			// get the texture from the model
			unsigned int diffuseTexture = m_fbxFile->getTextureByIndex(mesh->m_material->DiffuseTexture);
			// bid the texture and send it to our shader
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseTexture);
			glUniform1i(glGetUniformLocation(m_shader->m_program, "diffuseTexture"), 0);
			glUniform1f(glGetUniformLocation(m_shader->m_program, "lightAmbientStrength"),    lightSources[0]->getAmbientIntensity());
			glUniform3fv(glGetUniformLocation(m_shader->m_program, "lightSpecColor"), 1,	  &lightSources[0]->getSpecColor()[0]);
			glUniform3fv(glGetUniformLocation(m_shader->m_program, "lightPosition"), 1,		  &lightSources[0]->getPosition()[0]);
			glUniform3fv(glGetUniformLocation(m_shader->m_program, "lightColor"), 1,		  &lightSources[0]->getColour()[0]);
			glUniform1f(glGetUniformLocation(m_shader->m_program, "specPower"),				  lightSources[0]->getSpecIntensity());
			glUniform3fv(glGetUniformLocation(m_shader->m_program, "camPos"), 1,			  &m_camera->GetPos()[0]);
			//Pass through all lighting data,cam pos and diffuse and light the model accordingly.

			// draw the mesh
			glBindVertexArray(glData->vao);
			glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		glUseProgram(0);
	}
}

void FBXGameObject::DrawUI(float d_time)
{
	if (isAnimated) {

		ImGui::Begin("Animator");
		fbxCurrentFrame += d_time * 24;
		if (fbxCurrentFrame >= m_animation->m_endFrame)
		{
			fbxCurrentFrame = m_animation->m_startFrame;
		}
		std::string debugFrame = "Frame: " + std::to_string(fbxCurrentFrame);
		ImGui::Text(debugFrame.c_str());
		//ImGui::Checkbox("Show Bones", &showBones);
		if (ImGui::Button("Play All Pyro Animations"))
		{
			PlayAnimationTo(0, fbxFrameCount);
		}
		if (ImGui::Button("Pyro: Idle"))
		{
			PlayAnimationTo(1, 90);
		}
		if (ImGui::Button("Pyro: Take Dmg (Standing)"))
		{
			PlayAnimationTo(101, 160);
		}
		if (ImGui::Button("Pyro: Death (Standing)"))
		{
			PlayAnimationTo(171, 225);
		}
		if (ImGui::Button("Pyro: Crouch"))
		{
			PlayAnimationTo(235, 305);
		}
		if (ImGui::Button("Pyro: Take Dmg (Crouched)"))
		{
			PlayAnimationTo(316, 375);
		}
		if (ImGui::Button("Pyro: Death (Crouched)"))
		{
			PlayAnimationTo(386, 425);
		}
		if (ImGui::Button("Pyro: Shoot"))
		{
			PlayAnimationTo(436, 635);
		}
		if (ImGui::Button("Pyro: Reload (Standing)"))
		{
			PlayAnimationTo(646, 835);
		}
		if (ImGui::Button("Pyro: Reload (Crouched)"))
		{
			PlayAnimationTo(846, 985);
		}
		if (ImGui::Button("Pyro: Run"))
		{
			PlayAnimationTo(995, 1019);
		}
		ImGui::End();
	}
	//Drawing a simple animation key frame ui which allows you to play certain animations and displays the animation key frames.
}
