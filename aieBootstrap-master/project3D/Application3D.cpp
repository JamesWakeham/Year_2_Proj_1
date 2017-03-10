#include "Application3D.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

FlyCam flyCam;

GLint loc;

float refCoef = 1;
float rough = 0.2f;

Entity spear;
Entity spear2;
unsigned int m_programID,m_particleProgramID, m_bufferProgramID;

float vertPosterizeLevel = 100;

Application3D::Application3D() {

}

Application3D::~Application3D() {

}

void Application3D::FrameBufferStartup()
{
	/*
	// setup and bind a framebuffer 
	glGenFramebuffers(1, &m_FBO); 
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	// create a texture and bind it 
	glGenTextures(1, &m_fboTexture);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	// specify texture format for storage 
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// attach it to the framebuffer as the first colour attachment
	// the FBO MUST still be bound
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0);

	// /Depth Buffer Setup
	// setup and bind a 24bit depth buffer as a render buffer 
	glGenRenderbuffers(1, &m_fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720);
	// while the FBO is still bound
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);
	// Depth Buffer Setup/

	// we attach render targets here, code on following pages 
	// while the FBO is still bound 
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 }; 
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); 
	if (status != GL_FRAMEBUFFER_COMPLETE) 
		printf("Framebuffer Error!\n");

	// unbind the FBO so that we can render to the back buffer 
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	*/

	// setup framebuffer 
	glGenFramebuffers(1, &m_fbo); 
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); 
	glGenTextures(1, &m_fboTexture); 
	glBindTexture(GL_TEXTURE_2D, m_fboTexture); 
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0);
	glGenRenderbuffers(1, &m_fboDepth); 
	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);
	GLenum drawBuffers[] = 
	{ 
		GL_COLOR_ATTACHMENT0 
	};

	glDrawBuffers(1, drawBuffers); 
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	CREATE THE FULL SCREEN QUAD

	glm::vec2 texelSize = 1.0f / glm::vec2(1280, 720);
	// fullscreen quad 
	glm::vec2 halfTexel = 1.0f / glm::vec2(1280, 720) * 0.5f; 
	float vertexData[] = 
	{ 
		-1, -1, 0, 1, halfTexel.x, halfTexel.y, 
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
		-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,
		
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,

	}; 
	
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
}

bool Application3D::startup() {
	setBackgroundColour(0.2f, 0.21f, 0.2f);


	// create simple flying camera
	flyCam = FlyCam((float)getWindowHeight(), (float)getWindowWidth());

	sunPos = vec3(1);
	sunCol = vec3(1);
	
	LoadImageTextures();

	CreateTextureShader();
	CreateParticleShader();
	CreateBufferShader();

	LoadObj();


	CreateOpenGlBuffers(attribs, shapes);

	FrameBufferStartup();

	//modelPos = mat4(1);
	spear.Init(m_glInfo);
	spear2.Init(m_glInfo);

	spear.ConfigureBoundingSpheres(&attribs);
	spear2.ConfigureBoundingSpheres(&attribs);

	spear2.MoveBy(vec3(3, 0, 0));
	spear2.RotateBy(90, vec3(1, 1, 0));

	Gizmos::create(10000, 10000, 10000, 10000);


	m_emitter = new ParticleEmitter();
	m_emitter->Initialise(1000, 500, 0.1f, 1.0f, 1, 5, 1, 0.1f,3, glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1),vec3(0,-1,0),false);

	return true;
}

void Application3D::shutdown() {

	Gizmos::destroy();
}

void Application3D::CreateTextureShader()
{
	int success = GL_FALSE;

	std::ifstream in1("Shaders/vertex.txt");
	std::string vertShadContents((std::istreambuf_iterator<char>(in1)),
		std::istreambuf_iterator<char>());

	std::ifstream in2("Shaders/OrenNayarCookTorrence.txt");
	std::string fragShadContents((std::istreambuf_iterator<char>(in2)),
		std::istreambuf_iterator<char>());

	const char* vsSource = vertShadContents.c_str();
	const char* fsSource = fragShadContents.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);

	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog); 
		delete[] infoLog;
	}
	else {
		printf("Texture Shaders Compiled Successfully \n");
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Application3D::CreateParticleShader()
{
	int success = GL_FALSE;
	
		//particleVertex
	std::ifstream in3("Shaders/particleVertex.txt");
	std::string partVertShadContents((std::istreambuf_iterator<char>(in3)),
		std::istreambuf_iterator<char>());
	
		//particleFragment
	std::ifstream in4("Shaders/particleFragment.txt");
	std::string partFragShadContents((std::istreambuf_iterator<char>(in4)),
		std::istreambuf_iterator<char>());

	const char* partvsSource = partVertShadContents.c_str();
	const char* partfsSource = partFragShadContents.c_str();

	unsigned int partVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(partVertexShader, 1, (const char**)&partvsSource, 0);
	glCompileShader(partVertexShader);

	unsigned int partFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(partFragmentShader, 1, (const char**)&partfsSource, 0);
	glCompileShader(partFragmentShader);

	m_particleProgramID = glCreateProgram();
	glAttachShader(m_particleProgramID, partVertexShader);
	glAttachShader(m_particleProgramID, partFragmentShader);

	glLinkProgram(m_particleProgramID);

	glGetProgramiv(m_particleProgramID, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_particleProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_particleProgramID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog); 
		delete[] infoLog;
	}
	else {
		printf("Particle Shaders Compiled Successfully \n");
	}

	glDeleteShader(partVertexShader);
	glDeleteShader(partFragmentShader);
}

void Application3D::CreateBufferShader()
{
	int success = GL_FALSE;

	// m_bufferProgramID
	// buffer Vertex
	std::ifstream in3("Shaders/BufferVertex.txt");
	std::string partVertShadContents((std::istreambuf_iterator<char>(in3)),
		std::istreambuf_iterator<char>());

	// buffer Fragment
	std::ifstream in4("Shaders/BufferFragment.txt");
	std::string partFragShadContents((std::istreambuf_iterator<char>(in4)),
		std::istreambuf_iterator<char>());

	const char* partvsSource = partVertShadContents.c_str();
	const char* partfsSource = partFragShadContents.c_str();

	unsigned int bufferVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(bufferVertexShader, 1, (const char**)&partvsSource, 0);
	glCompileShader(bufferVertexShader);

	unsigned int buffFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(buffFragmentShader, 1, (const char**)&partfsSource, 0);
	glCompileShader(buffFragmentShader);

	m_bufferProgramID = glCreateProgram();
	glAttachShader(m_bufferProgramID, bufferVertexShader);
	glAttachShader(m_bufferProgramID, buffFragmentShader);

	glLinkProgram(m_bufferProgramID);

	glGetProgramiv(m_bufferProgramID, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_bufferProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_bufferProgramID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link buffer shader program!\n");
		printf("%s\n", infoLog); 
		delete[] infoLog;
	}
	else {
		printf("Buffer Shaders Compiled Successfully \n");
	}

	glDeleteShader(bufferVertexShader);
	glDeleteShader(buffFragmentShader);
}

void Application3D::LoadImageTextures()
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load("./models/soulspear/soulspear_diffuse.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	if (imageFormat == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else if (imageFormat == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	// now load the spec
	imageWidth = imageHeight = imageFormat = 0;
	data = stbi_load("./models/soulspear/soulspear_specular.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &specMap);
	glBindTexture(GL_TEXTURE_2D, specMap);
	if (imageFormat == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else if (imageFormat == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	// now load the spec
	imageWidth = imageHeight = imageFormat = 0;
	data = stbi_load("./textures/sparkle.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &specMap);
	glBindTexture(GL_TEXTURE_2D, specMap);
	if (imageFormat == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else if (imageFormat == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}

void Application3D::LoadObj()
{
	LoadObj("./models/soulspear/soulspear.obj");
}

void Application3D::LoadObj(const char * location)
{
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool success = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, location);
	if (success) {
		allShapes.resize(shapes.size());
		for (auto& shape : shapes)
		{
			allShapes.push_back(shape);
		}
	}
}

void Application3D::CreateOpenGlBuffers(tinyobj::attrib_t & attribs, std::vector<tinyobj::shape_t>& shapes)
{
	m_glInfo.resize(shapes.size());
	// grab each shape 
	int shapeIndex = 0;
	for (auto& shape : shapes)
	{
		// setup OpenGL data 
		glGenVertexArrays(1, &m_glInfo[shapeIndex].m_VAO);
		glGenBuffers(1, &m_glInfo[shapeIndex].m_VBO);
		glBindVertexArray(m_glInfo[shapeIndex].m_VAO);
		m_glInfo[shapeIndex].m_faceCount = shape.mesh.num_face_vertices.size();

		// collect triangle vertices 
		std::vector<OBJVertex> vertices;
		int index = 0;
		for (auto face : shape.mesh.num_face_vertices)
		{
			for (int i = 0; i < 3; ++i)
			{
				tinyobj::index_t idx = shape.mesh.indices[index + i];
				OBJVertex v = { 0 };
				// positions 
				v.x = attribs.vertices[3 * idx.vertex_index + 0];
				v.y = attribs.vertices[3 * idx.vertex_index + 1];
				v.z = attribs.vertices[3 * idx.vertex_index + 2];
				// normals 
				if (attribs.normals.size() > 0)
				{
					v.nx = attribs.normals[3 * idx.normal_index + 0];
					v.ny = attribs.normals[3 * idx.normal_index + 1];
					v.nz = attribs.normals[3 * idx.normal_index + 2];
				}
				// texture coordinates 
				if (attribs.texcoords.size() > 0)
				{
					v.u = attribs.texcoords[2 * idx.texcoord_index + 0];
					v.v = attribs.texcoords[2 * idx.texcoord_index + 1];
				}
				vertices.push_back(v);
			}
			index += face;
		}
		// bind vertex data 
		glBindBuffer(GL_ARRAY_BUFFER, m_glInfo[shapeIndex].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(OBJVertex), vertices.data(), GL_STATIC_DRAW);

		//position 
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), 0);

		//normal data 
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(OBJVertex), (void*)12); 

		//texture data 
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), (void*)24);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		shapeIndex++;
	}
}

void Application3D::update(float deltaTime) {
	
	ImGui::SliderFloat("Vertex Posterize Ammount",&vertPosterizeLevel,5,150);
	ImGui::Text("Left-Alt to toggle lock camera rotation");
	ImGui::Text("Left-Shift to move faster, Left-Ctrl to move slower");

	// query time since application started
	float time = getTime();

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	flyCam.Update(deltaTime);

	mat4 rotation;
	rotation = glm::rotate(rotation,deltaTime*0.3f, vec3(0, 1, 0));
	modelPos = rotation * modelPos;

	if (input->wasKeyPressed(aie::INPUT_KEY_R)) {
		rough = 1.0f;
		refCoef = 1.0f;
	}

	spear.RotateBy(deltaTime*0.3f, vec3(0, 1, 0));

	spear2.MoveTo(vec3(sin(getTime())*2, 0, 1));

	spear2.RotateBy(sin(getTime())*deltaTime, vec3(0, 1, 0));

	m_emitter->Update(deltaTime, flyCam.GetWorldTransform());

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application3D::draw() {	
	int loc;
	// wipe the gizmos clean for this frame
	Gizmos::clear();

	// bind our target 
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 1280, 720);
	// clear the target 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// draw our 3D scene 


	// sets oGL to linear texture Interpolation
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glUseProgram(m_programID);
	// tell the shader where the pointer holding the ProjectionView is
	loc = glGetUniformLocation(m_programID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(flyCam.GetProjectionView()[0][0]));

	// set texture slot 
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, m_texture); 
	
	int cameraPos = glGetUniformLocation(m_programID, "CameraPos");
	glUniform3fv(cameraPos, 1, &(flyCam.GetWorldTransform()[3][0]));

	// tell the shader where the pointer holding the ProjectionView is
	loc = glGetUniformLocation(m_programID, "vertPosterLevel");
	glUniform1f(loc, vertPosterizeLevel);

	RenderCulling(spear);
	RenderCulling(spear2);

	//m_particleProgramID
	glUseProgram(m_particleProgramID);
	loc = glGetUniformLocation(m_particleProgramID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(flyCam.GetProjectionView()[0][0]));
	m_emitter->Draw(m_particleProgramID);


	// draw our 3D scene 
	// gizmos for now, but replace with a 3D scene if desired 
	Gizmos::draw(flyCam.GetProjectionView()); 


	// bind the back-buffer 
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);

	// just clear the back-buffer depth as 
	// each pixel will be filled 
	glClear(GL_DEPTH_BUFFER_BIT);

	// draw our full-screen quad 
	glUseProgram(m_bufferProgramID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);

	loc = glGetUniformLocation(m_bufferProgramID, "target");
	glUniform1i(loc, 0);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

}

void Application3D::RenderCulling(Entity entity)
{
	vec4 planes[6]; 
	flyCam.getFrustumPlanes(flyCam.GetProjectionView(), planes);
	for (int i = 0; i < 6; i++)
	{
		float d = glm::dot(vec3(planes[i]), entity.sphere.centre) + planes[i].w;
		if (d < -entity.sphere.radius)
		{ 
			printf("Culled\n");
			break; 
		} else if (d < entity.sphere.radius)
		{ 
			entity.Draw(m_programID, &(sunPos[0]));
		} else
		{ 
			entity.Draw(m_programID, &(sunPos[0]));
		}
	}
}

unsigned int Application3D::GetProgromID()
{
	return m_programID;
}
