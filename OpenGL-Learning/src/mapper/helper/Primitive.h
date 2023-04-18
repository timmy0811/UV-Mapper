#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/normal.hpp"

namespace Helper {
	struct Vertex {
		glm::vec3 Position;
		glm::vec2 TexCoords;
		glm::vec3 Normal;
		float TexID;
		float reflection;
	};

	struct Sprite2DVertex {
		glm::vec2 Position;
		glm::vec2 TexCoords;
		float TexID;
	};

	struct LineVertex {
		glm::vec2 Position;
		glm::vec4 Color;
	};

	struct PositionVertex {
		glm::vec3 Position;
	};

	struct Face {
		Vertex vertices[4];
	};

	struct SymbolVertex {
		glm::vec2 Position;
		glm::vec2 uv;
		float background;
		float alpha;
	};
}