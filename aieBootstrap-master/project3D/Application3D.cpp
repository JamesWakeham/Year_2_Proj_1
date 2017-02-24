#include "Application3D.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <gl_core_4_4.h>
#include "Camera.h"
#include "FlyCam.h"
#include <stb_image.h>

#include "Object.h";
#include<iostream>
#include<fstream>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

FlyCam flyCam;

GLint loc;

float refCoef = 1;
float rough = 1;

//Model soulSpearModel = Model("./models/soulspear/soulspear.obj");
//Object soulSpear = Object(&soulSpearModel);

unsigned int m_programID;

Application3D::Application3D() {

}

Application3D::~Application3D() {

}

bool Application3D::startup() {
	
	setBackgroundColour(0.25f, 0.5f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple flying camera
	flyCam = FlyCam(getWindowHeight(), getWindowWidth());

	sunPos = vec3(1);
	sunCol = vec3(1);
	
	LoadImageTextures();
	//CreateShaders();
	CreateTextureShader();
	//GenerateTextureGrid();

	LoadObj();
	CreateOpenGlBuffers(attribs, shapes);
	modelPos = mat4(1);
	return true;
}

void Application3D::shutdown() {

	Gizmos::destroy();
}

void Application3D::CreateShaders()
{
	// create shaders 
	const char* vsSource =
		"#version 410\n \
		layout(location=0) in vec4 position; \
		layout(location=1) in vec4 colour; \
		out vec4 vColour; \
		uniform mat4 projectionViewWorldMatrix; \
		uniform float Scale; \
		vec4 scaleVec; \
		void main() { vColour = colour; gl_Position = projectionViewWorldMatrix * (position); }";
	
	const char* fsSource = 
		"#version 410\n \
		in vec4 vColour; \
		out vec4 fragColor; \
		void main() { fragColor = vColour; }"; 
	
	int success = GL_FALSE; 
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); 
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); 

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0); 
	glCompileShader(vertexShader); 
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
	} else {
		printf("Shaders Compiled Successfully");
	}
	glDeleteShader(fragmentShader); 
	glDeleteShader(vertexShader);


	loc = glGetUniformLocation(m_programID, "Scale");
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
		printf("Shaders Compiled Successfully");
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Application3D::GenerateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* aoVertices = new Vertex[rows * cols];
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			aoVertices[r * cols + c].position = vec4((float)c, 0, (float)r, 1);
			// create some arbitrary colour based off something 
			// that might not be related to tiling a texture 
			vec3 colour = vec3(sinf((c / (float)(cols - 1)) * (r / (float)(rows - 1))));
			aoVertices[r * cols + c].colour = vec4(colour, 1);
		}
	}
	// defining index count based off quad count (2 triangles per quad) 
	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];
	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			// triangle 1 
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			// triangle 2 
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	// Generate our GL Buffers 
	// Lets move these so that they are all generated together 
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	//Add the following line to generate a VertexArrayObject 
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	// create and bind budders to a vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (rows*cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	// ....Code Segment here to bind and fill VBO + IBO from previous pages 
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] aoVertices;
	delete[] auiIndices;
}

void Application3D::GenerateTextureGrid()
{
	float vertexData[] = { -5, 0, 5, 1, 0, 1, 5, 0, 5, 1, 1, 1, 5, 0, -5, 1, 1, 0, -5, 0, -5, 1, 0, 0, };
	unsigned int indexData[] = { 0, 1, 2, 0, 2, 3, };
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertexData, GL_STATIC_DRAW);
	glGenBuffers(1, &m_IBO); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

int i = 2;
void Application3D::DrawGrid(unsigned int rows, unsigned int cols)
{	
	
	//GLint loc = glGetUniformLocation(m_programID, "Scale");
	if (loc != -1)
	{
		glUniform1f(loc, i);
	}
	i += .0001f;
	// binds shaders to plane?
	glUseProgram(m_programID);
	unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(flyCam.GetProjectionView()));
	glBindVertexArray(m_VAO); 
	unsigned int indexCount = (rows - 1) * (cols - 1) * 6;

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Application3D::DrawGridWithTexture()
{
	// use our texture program 
	glUseProgram(m_programID); 
	// bind the camera 
	int loc = glGetUniformLocation(m_programID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(flyCam.GetProjectionView()[0][0])); 
	// set texture slot 
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, m_texture); 
	// tell the shader where it is 
	loc = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(loc, 0); 
	// draw 
	glBindVertexArray(m_VAO); 
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
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

void Application3D::LoadAllModels()
{

}


void Application3D::LoadObj()
{
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool success = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, "./models/soulspear/soulspear.obj");
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
		glEnableVertexAttribArray(0);

		//position 
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), 0);
		glEnableVertexAttribArray(1);
		//normal data 
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(OBJVertex), (void*)12); 
		glEnableVertexAttribArray(2);

		//texture data 
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), (void*)24);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		shapeIndex++;
	}
}

void Application3D::update(float deltaTime) {

	// query time since application started
	float time = getTime();

	// wipe the gizmos clean for this frame
	Gizmos::clear();

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

	//DrawGridWithTexture();

	// use our texture program 
	glUseProgram(m_programID); 
	// bind the camera 
	int loc = glGetUniformLocation(m_programID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(flyCam.GetProjectionView()[0][0]));

	// set texture slot 
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, m_texture); 

	// tell the shader where it is
	loc = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(loc, 0); 

	// set texture slot 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specMap);

	// tell the shader where it is
	loc = glGetUniformLocation(m_programID, "specMap");
	glUniform1i(loc, 1);

	// render Model
	loc = glGetUniformLocation(m_programID, "worldPositionMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(modelPos[0][0]));

	int roughness = glGetUniformLocation(m_programID, "roughness");
	glUniform1f(roughness, rough);
	// 0 = chrome, 1 = wood

	int reflectionCoefficient = glGetUniformLocation(m_programID, "reflectionCoefficient");
	glUniform1f(reflectionCoefficient,refCoef);
	// between 0 and 1

	int lightDir = glGetUniformLocation(m_programID, "LightDir");
	glUniform3fv(lightDir, 1, &(sunPos[0]));
	
	int cameraPos = glGetUniformLocation(m_programID, "CameraPos");
	glUniform3fv(cameraPos, 1, &(flyCam.GetWorldTransform()[3][0]));


	for (auto& gl : m_glInfo) 
	{
		glBindVertexArray(gl.m_VAO); 
		glDrawArrays(GL_TRIANGLES, 0, gl.m_faceCount * 3); 
	}



	Gizmos::draw(flyCam.GetProjectionView());
}

unsigned int Application3D::GetProgromID()
{
	return m_programID;
}
