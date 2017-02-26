#pragma once
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <glm/mat4x4.hpp>
#include <gl_core_4_4.h>
#include <vector>
#include <stb_image.h>
#include <iostream>
#include <fstream>

struct OpenGLInfo { unsigned int m_VAO; unsigned int m_VBO; unsigned int m_faceCount; };
struct OBJVertex { float x, y, z; float nx, ny, nz; float u, v; };

struct Particle { glm::vec3 position; glm::vec3 velocity; glm::vec4 colour; float size; float lifetime; float lifespan; };
struct ParticleVertex { glm::vec4 position; glm::vec4 colour; };