#pragma once
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <glm/mat4x4.hpp>
#include <gl_core_4_4.h>
#include <imgui_glfw3.h>
#include <vector>
#include <stb_image.h>
#include <iostream>
#include <fstream>
#include "BoundingSphere.h"
#include "tiny_obj_loader.h"

struct OpenGLInfo { unsigned int m_VAO; unsigned int m_VBO; unsigned int m_faceCount; };
struct OBJVertex { float x, y, z; float nx, ny, nz; float u, v; };

struct Particle { glm::vec3 position; glm::vec3 velocity; glm::vec4 colour; float size; float lifetime; float lifespan; float rotation; };
struct ParticleVertex { glm::vec4 position; glm::vec4 colour; glm::vec2 uv; };

struct GPUParticle { GPUParticle() : lifetime(1), lifespan(0) {} glm::vec3 position; glm::vec3 velocity; float lifetime; float lifespan; };