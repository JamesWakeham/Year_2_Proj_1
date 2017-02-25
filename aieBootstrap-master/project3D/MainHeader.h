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

struct Particle { vec3 position; vec3 velocity; vec4 colour; float size; float lifetime; float lifespan; };
struct ParticleVertex { vec4 position; vec4 colour; };