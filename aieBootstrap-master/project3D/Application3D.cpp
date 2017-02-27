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
unsigned int m_programID,m_particleProgramID;

Application3D::Application3D() {

}

Application3D::~Application3D() {

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

	LoadObj();

	CreateOpenGlBuffers(attribs, shapes);

	//modelPos = mat4(1);
	spear.Init(m_glInfo);
	spear2.Init(m_glInfo);

	spear2.MoveBy(vec3(3, 0, 0));
	spear2.RotateBy(90, vec3(1, 1, 0));

	m_emitter = new ParticleEmitter();
	m_emitter->Initialise(1000, 500, 0.1f, 1.0f, 1, 5, 1, 0.1f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1));

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
		printf("%s\n", infoLog); delete[] infoLog;
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
		printf("%s\n", infoLog); delete[] infoLog;
	}
	else {
		printf("Particle Shaders Compiled Successfully \n");
	}

	glDeleteShader(partVertexShader);
	glDeleteShader(partFragmentShader);
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

	if (input->wasKeyPressed(aie::INPUT_KEY_UP)) {
		rough += 0.05f;
	}
	if (input->wasKeyPressed(aie::INPUT_KEY_DOWN)) {
		rough -= 0.05f;
	}
	if (input->wasKeyPressed(aie::INPUT_KEY_RIGHT)) {
		refCoef += 0.05f;
	}
	if (input->wasKeyPressed(aie::INPUT_KEY_LEFT)) {
		refCoef -= 0.05f;
	}
	if (refCoef > 1)refCoef = 1;
	if (rough > 1)rough = 1;

	if (refCoef < 0)refCoef = 0;
	if (rough < 0)rough = 0;
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application3D::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// tell the shader where the pointer holding the ProjectionView is
	int loc = glGetUniformLocation(m_programID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(flyCam.GetProjectionView()[0][0]));

	// set texture slot 
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, m_texture); 
	
	int cameraPos = glGetUniformLocation(m_programID, "CameraPos");
	glUniform3fv(cameraPos, 1, &(flyCam.GetWorldTransform()[3][0]));

	glUseProgram(m_programID);
	spear2.Draw(m_programID, &(sunPos[0]));
	spear.Draw(m_programID, &(sunPos[0]));

	//m_particleProgramID
	glUseProgram(m_particleProgramID);
	loc = glGetUniformLocation(m_particleProgramID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(flyCam.GetProjectionView()[0][0]));
	m_emitter->Draw();

	glUseProgram(m_programID);
}

unsigned int Application3D::GetProgromID()
{
	return m_programID;
}
